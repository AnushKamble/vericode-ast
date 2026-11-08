import pytest
from pathlib import Path
from vericode.bridge import VerifierBridge
from vericode.prompt_synthesizer import PromptSynthesizer
from vericode.repair_orchestrator import RepairOrchestrator
from vericode.llm_client import get_llm_client


@pytest.fixture
def bridge():
    return VerifierBridge()


def test_clean_raii_passes(bridge):
    safe_code = """
    #include <memory>
    #include <iostream>
    void compute() {
        auto buf = std::make_unique<int[]>(1024);
        buf[0] = 42;
        std::cout << buf[0] << std::endl;
    }
    """
    res = bridge.verify_source(safe_code)
    assert res["passed"] is True
    assert res["diagnostic_count"] == 0


def test_cwe401_memory_leak_detection(bridge):
    buggy_code = """
    #include <stdlib.h>
    void process() {
        int* data = (int*)malloc(sizeof(int) * 100);
        data[0] = 1;
        // missing free
    }
    """
    res = bridge.verify_source(buggy_code)
    assert res["passed"] is False
    assert res["diagnostic_count"] > 0
    assert any("CWE-401" in d.get("rule_id", "") for d in res["diagnostics"])


def test_cwe362_race_condition_detection(bridge):
    buggy_code = """
    #include <thread>
    int counter = 0;
    void worker() {
        std::thread t([]{ counter++; });
        t.join();
    }
    """
    res = bridge.verify_source(buggy_code)
    assert res["passed"] is False
    assert any("CWE-362" in d.get("rule_id", "") for d in res["diagnostics"])


def test_cwe476_null_dereference_detection(bridge):
    buggy_code = """
    struct Node { int val; };
    void test(Node* head) {
        int v = head->val;
    }
    """
    res = bridge.verify_source(buggy_code)
    assert res["passed"] is False
    assert any("CWE-476" in d.get("rule_id", "") for d in res["diagnostics"])


def test_prompt_synthesizer(bridge):
    buggy_code = """
    void leak() {
        int* ptr = (int*)malloc(40);
    }
    """
    res = bridge.verify_source(buggy_code)
    prompt = PromptSynthesizer.construct_repair_prompt(buggy_code, res["diagnostics"])
    assert "CWE-401" in prompt
    assert "Memory Leak" in prompt
    assert "```cpp" in prompt


def test_deterministic_self_repair(bridge):
    orchestrator = RepairOrchestrator(verifier_bridge=bridge, max_iterations=3)
    llm = get_llm_client("deterministic")
    buggy_code = """
    void leak() {
        int* ptr = (int*)malloc(40);
    }
    """
    result = orchestrator.run_repair_loop(buggy_code, llm)
    assert result["status"] in ["SUCCESS", "CONVERGED_CLEAN"]
    assert "make_unique" in result["final_code"] or "unique_ptr" in result["final_code"] or "free" in result["final_code"]


def test_benchmark_datasets_exist():
    base_dir = Path(__file__).resolve().parent.parent / "benchmarks" / "datasets"
    assert base_dir.exists()
    cpp_files = list(base_dir.rglob("*.cpp"))
    assert len(cpp_files) >= 40
