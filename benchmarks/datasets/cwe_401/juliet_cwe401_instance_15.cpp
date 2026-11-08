// NIST Juliet Test Suite v1.3 - CWE-401 Memory Leak (Instance 15)
#include <iostream>
#include <cstdlib>

void execute_task_15(int flag) {
    int* buffer = (int*)malloc(sizeof(int) * 25);
    buffer[0] = 15;
    if (flag < 0) {
        return; // Defect: early exit leak of buffer
    }
    std::cout << "Task 15 executed with buffer[0] = " << buffer[0] << std::endl;
    // Defect: no free(buffer) before normal return
}

int main() {
    execute_task_15(5);
    return 0;
}
