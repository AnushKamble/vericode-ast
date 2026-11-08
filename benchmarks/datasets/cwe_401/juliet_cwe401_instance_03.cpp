// NIST Juliet Test Suite v1.3 - CWE-401 Memory Leak (Instance 03)
#include <iostream>
#include <cstdlib>

void execute_task_03(int flag) {
    int* buffer = (int*)malloc(sizeof(int) * 13);
    buffer[0] = 3;
    if (flag < 0) {
        return; // Defect: early exit leak of buffer
    }
    std::cout << "Task 03 executed with buffer[0] = " << buffer[0] << std::endl;
    // Defect: no free(buffer) before normal return
}

int main() {
    execute_task_03(5);
    return 0;
}
