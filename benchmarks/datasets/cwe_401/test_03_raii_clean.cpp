// Test Case: Clean RAII Pattern (Zero Defect Baseline)
#include <iostream>
#include <memory>

void safe_process_buffer() {
    auto data = std::make_unique<int[]>(100);
    for (int i = 0; i < 100; ++i) {
        data[i] = i * 2;
    }
    std::cout << "Clean RAII buffer initialized, head = " << data[0] << std::endl;
}

int main() {
    safe_process_buffer();
    return 0;
}
