// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 06)
#include <iostream>
#include <thread>
#include <vector>

int counter_06 = 0;

void worker_06() {
    for (int j = 0; j < 500; ++j) {
        counter_06++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_06);
    std::thread t2(worker_06);
    t1.join();
    t2.join();
    std::cout << "Final counter_06 = " << counter_06 << std::endl;
    return 0;
}
