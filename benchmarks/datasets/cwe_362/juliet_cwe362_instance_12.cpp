// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 12)
#include <iostream>
#include <thread>
#include <vector>

int counter_12 = 0;

void worker_12() {
    for (int j = 0; j < 500; ++j) {
        counter_12++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_12);
    std::thread t2(worker_12);
    t1.join();
    t2.join();
    std::cout << "Final counter_12 = " << counter_12 << std::endl;
    return 0;
}
