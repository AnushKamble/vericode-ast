// Test Case: CWE-401 Raw Heap Allocation Without Deallocation
#include <iostream>

void process_buffer() {
    int* data = new int[100];
    for (int i = 0; i < 100; ++i) {
        data[i] = i * 2;
    }
    std::cout << "Buffer initialized, head = " << data[0] << std::endl;
    // Defect: data is never freed or deleted; memory leaks upon function return
}

int main() {
    process_buffer();
    return 0;
}
