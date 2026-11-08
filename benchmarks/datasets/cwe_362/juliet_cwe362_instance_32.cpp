// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 32)
#include <iostream>
#include <thread>
#include <vector>

int counter_32 = 0;

void worker_32() {
    for (int j = 0; j < 500; ++j) {
        counter_32++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_32);
    std::thread t2(worker_32);
    t1.join();
    t2.join();
    std::cout << "Final counter_32 = " << counter_32 << std::endl;
    return 0;
}
