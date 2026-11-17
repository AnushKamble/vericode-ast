import pytest
from vericode.bridge import VerifierBridge
from vericode.repair_orchestrator import RepairOrchestrator
from vericode.llm_client import get_llm_client


@pytest.fixture
def bridge():
    return VerifierBridge()


def test_real_world_cve_401_linux_leak(bridge):
    """Real-world pattern inspired by Linux kernel drivers net/ethernet buffer leak."""
    code = """
    #include <stdlib.h>
    int eth_send_packet(int len) {
        char* pkt_buf = (char*)malloc(len);
        if (len <= 0) {
            return -1; // Leaks pkt_buf on error return path
        }
        // do send
        free(pkt_buf);
        return 0;
    }
    """
    res = bridge.verify_source(code)
    assert res["passed"] is False
    assert any("CWE-401" in d.get("rule_id", "") for d in res["diagnostics"])


def test_real_world_cve_362_redis_race(bridge):
    """Real-world pattern inspired by Redis asynchronous replication worker race."""
    code = """
    #include <thread>
    static long long shared_repl_offset = 0;
    void sync_worker() {
        std::thread t([]{
            shared_repl_offset += 1024; // Unsynchronized write to shared replication offset
        });
        t.join();
    }
    """
    res = bridge.verify_source(code)
    assert res["passed"] is False
    assert any("CWE-362" in d.get("rule_id", "") for d in res["diagnostics"])


def test_real_world_cve_476_git_null_deref(bridge):
    """Real-world pattern inspired by Git tree parsing unchecked object dereference."""
    code = """
    struct object_entry { int flags; char* hash; };
    int parse_tree_entry(struct object_entry* entry) {
        int f = entry->flags; // Unchecked pointer dereference before null guard
        if (!entry) return -1;
        return f;
    }
    """
    res = bridge.verify_source(code)
    assert res["passed"] is False
    assert any("CWE-476" in d.get("rule_id", "") for d in res["diagnostics"])


def test_real_world_self_repair_convergence(bridge):
    """Verify that deterministic repair orchestrator remediates real-world error path leak."""
    orchestrator = RepairOrchestrator(verifier_bridge=bridge, max_iterations=3)
    llm = get_llm_client("deterministic")
    code = """
    #include <stdlib.h>
    void process_stream(int sz) {
        int* buf = (int*)malloc(sz * sizeof(int));
        if (sz < 0) {
            return;
        }
        free(buf);
    }
    """
    result = orchestrator.run_repair_loop(code, llm)
    assert result["status"] in ["SUCCESS", "CONVERGED_CLEAN"]
