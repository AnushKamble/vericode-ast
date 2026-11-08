// NIST Juliet Test Suite v1.3 - CWE-401 Memory Leak (Instance 11)
#include <iostream>
#include <cstdlib>

void execute_task_11(int flag) {
    int* buffer = (int*)malloc(sizeof(int) * 21);
    buffer[0] = 11;
    if (flag < 0) {
        return; // Defect: early exit leak of buffer
    }
    std::cout << "Task 11 executed with buffer[0] = " << buffer[0] << std::endl;
    // Defect: no free(buffer) before normal return
}

int main() {
    execute_task_11(5);
    return 0;
}
