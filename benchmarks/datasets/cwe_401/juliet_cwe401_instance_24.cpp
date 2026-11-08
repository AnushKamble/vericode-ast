// NIST Juliet Test Suite v1.3 - CWE-401 Memory Leak (Instance 24)
#include <iostream>
#include <cstdlib>

void execute_task_24(int flag) {
    int* buffer = (int*)malloc(sizeof(int) * 34);
    buffer[0] = 24;
    if (flag < 0) {
        return; // Defect: early exit leak of buffer
    }
    std::cout << "Task 24 executed with buffer[0] = " << buffer[0] << std::endl;
    // Defect: no free(buffer) before normal return
}

int main() {
    execute_task_24(5);
    return 0;
}
