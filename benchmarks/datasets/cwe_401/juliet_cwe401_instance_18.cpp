// NIST Juliet Test Suite v1.3 - CWE-401 Memory Leak (Instance 18)
#include <iostream>
#include <cstdlib>

void execute_task_18(int flag) {
    int* buffer = (int*)malloc(sizeof(int) * 28);
    buffer[0] = 18;
    if (flag < 0) {
        return; // Defect: early exit leak of buffer
    }
    std::cout << "Task 18 executed with buffer[0] = " << buffer[0] << std::endl;
    // Defect: no free(buffer) before normal return
}

int main() {
    execute_task_18(5);
    return 0;
}
