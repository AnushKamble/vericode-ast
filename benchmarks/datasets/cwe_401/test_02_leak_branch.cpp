// Test Case: CWE-401 Raw malloc with early exit branch leak
#include <iostream>
#include <cstdlib>

int process_packet(int flag) {
    int* ptr = (int*)malloc(sizeof(int) * 10);
    if (flag < 0) {
        return -1; // Defect: Leaks heap memory allocated to ptr on negative flag
    }
    ptr[0] = flag;
    int res = ptr[0];
    return res;
}

int main() {
    process_packet(10);
    return 0;
}
