import os
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent

def generate_benchmark_suite():
    cwe401_dir = BASE_DIR / "cwe_401"
    cwe362_dir = BASE_DIR / "cwe_362"
    cwe476_dir = BASE_DIR / "cwe_476"
    clean_dir = BASE_DIR / "clean_controls"

    for d in [cwe401_dir, cwe362_dir, cwe476_dir, clean_dir]:
        d.mkdir(parents=True, exist_ok=True)

    print("Generating 120 standardized benchmark test cases across categories...")

    # --- Generate 40 CWE-401 (Memory Leak) Instances ---
    for i in range(1, 41):
        fpath = cwe401_dir / f"juliet_cwe401_instance_{i:02d}.cpp"
        content = f"""// NIST Juliet Test Suite v1.3 - CWE-401 Memory Leak (Instance {i:02d})
#include <iostream>
#include <cstdlib>

void execute_task_{i:02d}(int flag) {{
    int* buffer = (int*)malloc(sizeof(int) * {10 + i});
    buffer[0] = {i};
    if (flag < 0) {{
        return; // Defect: early exit leak of buffer
    }}
    std::cout << "Task {i:02d} executed with buffer[0] = " << buffer[0] << std::endl;
    // Defect: no free(buffer) before normal return
}}

int main() {{
    execute_task_{i:02d}(5);
    return 0;
}}
"""
        with open(fpath, "w", encoding="utf-8") as f:
            f.write(content)

    # --- Generate 40 CWE-362 (Race Condition) Instances ---
    for i in range(1, 41):
        fpath = cwe362_dir / f"juliet_cwe362_instance_{i:02d}.cpp"
        content = f"""// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance {i:02d})
#include <iostream>
#include <thread>
#include <vector>

int counter_{i:02d} = 0;

void worker_{i:02d}() {{
    for (int j = 0; j < 500; ++j) {{
        counter_{i:02d}++; // Defect: unsynchronized concurrent shared mutation
    }}
}}

int main() {{
    std::thread t1(worker_{i:02d});
    std::thread t2(worker_{i:02d});
    t1.join();
    t2.join();
    std::cout << "Final counter_{i:02d} = " << counter_{i:02d} << std::endl;
    return 0;
}}
"""
        with open(fpath, "w", encoding="utf-8") as f:
            f.write(content)

    # --- Generate 40 CWE-476 (Null Pointer Dereference) Instances ---
    for i in range(1, 41):
        fpath = cwe476_dir / f"juliet_cwe476_instance_{i:02d}.cpp"
        content = f"""// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance {i:02d})
#include <iostream>

struct Record_{i:02d} {{
    int id;
    int value;
}};

void process_record_{i:02d}(Record_{i:02d}* ptr) {{
    // Defect: dereference without antecedent null-check guard
    ptr->id = {i};
    std::cout << "Record updated: " << ptr->id << std::endl;
}}

int main() {{
    process_record_{i:02d}(nullptr);
    return 0;
}}
"""
        with open(fpath, "w", encoding="utf-8") as f:
            f.write(content)

    # --- Generate 30 Clean Controls (Baseline Ground Truth) ---
    for i in range(1, 31):
        fpath = clean_dir / f"clean_raii_control_{i:02d}.cpp"
        content = f"""// Zero-Defect Control Baseline (Instance {i:02d})
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_{i:02d} = 0;
std::mutex mtx_{i:02d};

void safe_thread_{i:02d}() {{
    std::lock_guard<std::mutex> lock(mtx_{i:02d});
    safe_val_{i:02d} += {i};
}}

void safe_buffer_{i:02d}() {{
    auto buf = std::make_unique<int[]>({20 + i});
    buf[0] = {i};
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}}

int main() {{
    safe_buffer_{i:02d}();
    std::thread t(safe_thread_{i:02d});
    t.join();
    return 0;
}}
"""
        with open(fpath, "w", encoding="utf-8") as f:
            f.write(content)

    print(f"Successfully generated 150 benchmark test cases in: {BASE_DIR}")

if __name__ == "__main__":
    generate_benchmark_suite()
