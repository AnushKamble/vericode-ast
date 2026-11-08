// NIST Juliet Test Suite v1.3 - CWE-401 Memory Leak (Instance 34)
#include <iostream>
#include <cstdlib>

void execute_task_34(int flag) {
    int* buffer = (int*)malloc(sizeof(int) * 44);
    buffer[0] = 34;
    if (flag < 0) {
        return; // Defect: early exit leak of buffer
    }
    std::cout << "Task 34 executed with buffer[0] = " << buffer[0] << std::endl;
    // Defect: no free(buffer) before normal return
}

int main() {
    execute_task_34(5);
    return 0;
}
