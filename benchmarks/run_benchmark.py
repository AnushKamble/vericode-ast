import os
import sys
import time
import argparse
from pathlib import Path
import numpy as np

# Add python path
sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "python"))

from vericode.bridge import VerifierBridge
from vericode.repair_orchestrator import RepairOrchestrator
from vericode.llm_client import get_llm_client

def run_suite(backend: str = "deterministic", model_name: str = "qwen3:8b", limit: int = None):
    base_dir = Path(__file__).resolve().parent / "datasets"
    bridge = VerifierBridge()
    orchestrator = RepairOrchestrator(bridge, max_iterations=3)
    llm_client = get_llm_client(backend=backend, model_name=model_name)

    print("================================================================================")
    print(" VeriCode-AST Empirical Evaluation Suite (ICSE / FSE Benchmark Harness)")
    print(f" Execution Backend: {backend.upper()} (Model: {model_name})")
    print("================================================================================")

    test_files = sorted(list(base_dir.rglob("*.cpp")))
    if limit:
        test_files = test_files[:limit]

    print(f"Discovered {len(test_files)} empirical benchmark instances.\n")

    results = []
    latencies = []
    repair_times = []

    for idx, f_path in enumerate(test_files, 1):
        rel_name = f_path.relative_to(base_dir)
        is_clean_control = "clean" in f_path.stem or "clean_controls" in str(f_path)

        # Measure AST static analysis latency
        t0 = time.perf_counter()
        verification = bridge.verify_file(str(f_path))
        elapsed_us = (time.perf_counter() - t0) * 1_000_000
        latencies.append(elapsed_us)

        initial_passed = verification["passed"]
        initial_diags = verification["diagnostic_count"]

        repair_status = "N/A (Clean Control)"
        final_passed = initial_passed
        repair_duration_ms = 0.0

        if not initial_passed:
            with open(f_path, "r", encoding="utf-8") as f:
                buggy_code = f.read()

            t_rep = time.perf_counter()
            repair_res = orchestrator.run_repair_loop(buggy_code, llm_client)
            repair_duration_ms = (time.perf_counter() - t_rep) * 1000
            repair_times.append(repair_duration_ms)

            repair_status = f"{repair_res['status']} ({repair_res['total_iterations']} turns)"
            final_passed = (repair_res["status"] in ["SUCCESS", "CONVERGED_CLEAN"])

        results.append({
            "file": str(rel_name),
            "is_clean": is_clean_control,
            "initial_passed": initial_passed,
            "initial_diags": initial_diags,
            "final_passed": final_passed,
            "repair_status": repair_status,
            "latency_us": elapsed_us,
            "repair_duration_ms": repair_duration_ms
        })

        if idx % 25 == 0 or idx == len(test_files):
            print(f"  Processed [{idx}/{len(test_files)}] test cases... (Latest: {rel_name})")

    # Compute Statistical Confusion Matrix
    true_positives = 0
    false_positives = 0
    true_negatives = 0
    false_negatives = 0
    repaired_successfully = 0
    total_buggy = 0

    # Group results by empirical category
    cat_stats = {
        "CWE-401 (Memory Leak)": {"tp": 0, "fp": 0, "tn": 0, "fn": 0, "repaired": 0, "buggy": 0, "lat": []},
        "CWE-362 (Race Condition)": {"tp": 0, "fp": 0, "tn": 0, "fn": 0, "repaired": 0, "buggy": 0, "lat": []},
        "CWE-476 (Null Dereference)": {"tp": 0, "fp": 0, "tn": 0, "fn": 0, "repaired": 0, "buggy": 0, "lat": []},
        "Clean Control Baselines": {"tp": 0, "fp": 0, "tn": 0, "fn": 0, "repaired": 0, "buggy": 0, "lat": []}
    }

    for r in results:
        if "cwe_401" in r["file"]:
            cat = "CWE-401 (Memory Leak)"
        elif "cwe_362" in r["file"]:
            cat = "CWE-362 (Race Condition)"
        elif "cwe_476" in r["file"]:
            cat = "CWE-476 (Null Dereference)"
        else:
            cat = "Clean Control Baselines"

        st = cat_stats[cat]
        st["lat"].append(r["latency_us"])

        if r["is_clean"]:
            if r["initial_passed"]:
                true_negatives += 1
                st["tn"] += 1
            else:
                false_positives += 1
                st["fp"] += 1
        else:
            total_buggy += 1
            st["buggy"] += 1
            if not r["initial_passed"]:
                true_positives += 1
                st["tp"] += 1
            else:
                false_negatives += 1
                st["fn"] += 1

            if r["final_passed"]:
                repaired_successfully += 1
                st["repaired"] += 1

    precision = (true_positives / (true_positives + false_positives)) * 100 if (true_positives + false_positives) > 0 else 0
    recall = (true_positives / (true_positives + false_negatives)) * 100 if (true_positives + false_negatives) > 0 else 0
    specificity = (true_negatives / (true_negatives + false_positives)) * 100 if (true_negatives + false_positives) > 0 else 0
    f1 = 2 * (precision * recall) / (precision + recall) if (precision + recall) > 0 else 0
    repair_rate = (repaired_successfully / total_buggy) * 100 if total_buggy > 0 else 0

    p50_latency = float(np.percentile(latencies, 50))
    p95_latency = float(np.percentile(latencies, 95))
    p99_latency = float(np.percentile(latencies, 99))
    avg_latency = float(np.mean(latencies))

    print("\n================================================================================")
    print(" EMPIRICAL PERFORMANCE EVALUATION SUMMARY (ACM SIGSOFT Empirical Standard)")
    print("================================================================================")
    print(f" Total Benchmark Instances       : {len(results)}")
    print(f" Vulnerability Instances (Buggy) : {total_buggy}")
    print(f" Clean Control Baselines         : {true_negatives + false_positives}")
    print("--------------------------------------------------------------------------------")
    print(f" True Positives (TP)             : {true_positives}")
    print(f" False Positives (FP)            : {false_positives}")
    print(f" True Negatives (TN)             : {true_negatives}")
    print(f" False Negatives (FN)            : {false_negatives}")
    print("--------------------------------------------------------------------------------")
    print(f" Defect Detection Precision      : {precision:.2f}%")
    print(f" Defect Detection Recall         : {recall:.2f}%")
    print(f" Specificity (Clean Pass Rate)   : {specificity:.2f}%")
    print(f" F1-Score                        : {f1:.2f}%")
    print(f" Neuro-Symbolic Self-Repair Rate : {repair_rate:.2f}% ({repaired_successfully}/{total_buggy} resolved)")
    print("--------------------------------------------------------------------------------")
    print(f" Latency Mean                    : {avg_latency:.1f} us")
    print(f" Latency P50 (Median)            : {p50_latency:.1f} us")
    print(f" Latency P95                     : {p95_latency:.1f} us")
    print(f" Latency P99                     : {p99_latency:.1f} us")
    print("================================================================================")

    # Build dynamic rows for LaTeX table
    table_rows = []
    for cat_name, st in cat_stats.items():
        count = len(st["lat"])
        c_tp = st["tp"]
        c_fp = st["fp"]
        c_fn = st["fn"]
        c_buggy = st["buggy"]
        c_repaired = st["repaired"]
        c_prec = (c_tp / (c_tp + c_fp) * 100.0) if (c_tp + c_fp) > 0 else (100.0 if st["tn"] > 0 else 0.0)
        c_rec = (c_tp / (c_tp + c_fn) * 100.0) if (c_tp + c_fn) > 0 else (100.0 if st["tn"] > 0 else 0.0)
        c_rep_str = f"{(c_repaired / c_buggy * 100.0):.1f}\\%" if c_buggy > 0 else "N/A"
        c_avg_lat = float(np.mean(st["lat"])) if st["lat"] else 0.0
        table_rows.append(f"{cat_name:<28} & {count:<3} & {c_prec:.1f}\\% & {c_rec:.1f}\\% & {c_rep_str} & {c_avg_lat:.1f}\\,$\\mu$s \\\\")

    rows_str = "\n".join(table_rows)

    # Export LaTeX table snippet for paper
    paper_dir = Path(__file__).resolve().parent.parent / "paper"
    latex_table = f"""% Auto-generated empirical results table for VeriCode-AST
\\begin{{table}}[t]
\\centering
\\caption{{Empirical Invariant Detection and Self-Repair Performance ({len(results)} Benchmark Instances)}}
\\label{{tab:empirical_results}}
\\resizebox{{\\columnwidth}}{{!}}{{%
\\begin{{tabular}}{{lccccc}}
\\toprule
\\textbf{{Benchmark Category}} & \\textbf{{Instances}} & \\textbf{{Precision}} & \\textbf{{Recall}} & \\textbf{{Repair Conv.}} & \\textbf{{Avg. Latency}} \\\\
\\midrule
{rows_str}
\\midrule
\\textbf{{Aggregate System}} & \\textbf{{{len(results)}}} & \\textbf{{{precision:.1f}\\%}} & \\textbf{{{recall:.1f}\\%}} & \\textbf{{{repair_rate:.1f}\\%}} & \\textbf{{{avg_latency:.1f}\\,$\\mu$s}} \\\\
\\bottomrule
\\end{{tabular}}%
}}
\\end{{table}}
"""
    with open(paper_dir / "results_table.tex", "w", encoding="utf-8") as f:
        f.write(latex_table)
    print(f"Exported LaTeX results table to: {paper_dir / 'results_table.tex'}")

    return 0 if (precision >= 98.0 and recall >= 98.0 and repair_rate >= 95.0) else 1

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="VeriCode-AST Empirical Benchmark Evaluator")
    parser.add_argument("--backend", default="deterministic", choices=["deterministic", "ollama"],
                        help="LLM inference backend: 'deterministic' (sub-millisecond rule engine) or 'ollama' (live local daemon)")
    parser.add_argument("--model", default="qwen3:8b", help="Model name for Ollama backend (default: qwen3:8b)")
    parser.add_argument("--limit", type=int, default=None, help="Optional limit on number of test cases to run")
    args = parser.parse_args()

    sys.exit(run_suite(backend=args.backend, model_name=args.model, limit=args.limit))
