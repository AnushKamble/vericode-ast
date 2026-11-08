// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 14)
#include <iostream>
#include <thread>
#include <vector>

int counter_14 = 0;

void worker_14() {
    for (int j = 0; j < 500; ++j) {
        counter_14++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_14);
    std::thread t2(worker_14);
    t1.join();
    t2.join();
    std::cout << "Final counter_14 = " << counter_14 << std::endl;
    return 0;
}
