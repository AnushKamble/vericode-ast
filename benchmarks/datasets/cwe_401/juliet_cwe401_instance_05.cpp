// NIST Juliet Test Suite v1.3 - CWE-401 Memory Leak (Instance 05)
#include <iostream>
#include <cstdlib>

void execute_task_05(int flag) {
    int* buffer = (int*)malloc(sizeof(int) * 15);
    buffer[0] = 5;
    if (flag < 0) {
        return; // Defect: early exit leak of buffer
    }
    std::cout << "Task 05 executed with buffer[0] = " << buffer[0] << std::endl;
    // Defect: no free(buffer) before normal return
}

int main() {
    execute_task_05(5);
    return 0;
}
