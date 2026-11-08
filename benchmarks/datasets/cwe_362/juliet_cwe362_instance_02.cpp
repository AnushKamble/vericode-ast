// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 02)
#include <iostream>
#include <thread>
#include <vector>

int counter_02 = 0;

void worker_02() {
    for (int j = 0; j < 500; ++j) {
        counter_02++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_02);
    std::thread t2(worker_02);
    t1.join();
    t2.join();
    std::cout << "Final counter_02 = " << counter_02 << std::endl;
    return 0;
}
