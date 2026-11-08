import sys
import os
import argparse
from vericode.bridge import VerifierBridge
from vericode.repair_orchestrator import RepairOrchestrator
from vericode.llm_client import get_llm_client

def main():
    parser = argparse.ArgumentParser(description="VeriCode-AST: Neuro-Symbolic Code Self-Repair CLI")
    parser.add_argument("input_file", help="Path to input C++ source file")
    parser.add_argument("--backend", choices=["deterministic", "ollama"], default="deterministic",
                        help="LLM inference backend: 'ollama' (local daemon) or 'deterministic' (sub-millisecond rule engine)")
    parser.add_argument("--model", default="qwen3:8b", help="Model name for Ollama (default: qwen3:8b)")
    parser.add_argument("--max-turns", type=int, default=3, help="Maximum repair iterations")
    parser.add_argument("--output", "-o", help="Optional output path for repaired file")
    args = parser.parse_args()

    if not os.path.exists(args.input_file):
        print(f"Error: Target file not found: {args.input_file}")
        sys.exit(1)

    with open(args.input_file, "r", encoding="utf-8") as f:
        initial_code = f.read()

    print("================================================================================")
    print(" VeriCode-AST Neuro-Symbolic Self-Repair Orchestrator")
    print(f" Target Source : {args.input_file}")
    print(f" Backend Engine: {args.backend.upper()} (Model: {args.model})")
    print("================================================================================")

    bridge = VerifierBridge()
    orchestrator = RepairOrchestrator(bridge, max_iterations=args.max_turns)
    llm_client = get_llm_client(backend=args.backend, model_name=args.model)

    result = orchestrator.run_repair_loop(initial_code, llm_client)

    print(f"Status           : {result['status']}")
    print(f"Total Iterations : {result['total_iterations']}")
    print(f"Total Duration   : {result.get('total_duration_ms', 0):.2f} ms\n")

    for record in result["history"]:
        step = record["iteration"]
        passed = record["passed"]
        count = record["diagnostic_count"]
        latency = record.get("verify_latency_us", 0)
        status_str = "[CLEAN]" if passed else f"[FAILED: {count} violations]"
        print(f"  Iteration {step}: {status_str:<24} (AST Latency: {latency:.1f} us)")

    if args.output:
        with open(args.output, "w", encoding="utf-8") as f:
            f.write(result["final_code"])
        print(f"\nRepaired artifact written to: {args.output}")

    sys.exit(0 if result["status"] in ["SUCCESS", "CONVERGED_CLEAN"] else 2)

if __name__ == "__main__":
    main()
