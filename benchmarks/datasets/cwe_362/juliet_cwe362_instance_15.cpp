// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 15)
#include <iostream>
#include <thread>
#include <vector>

int counter_15 = 0;

void worker_15() {
    for (int j = 0; j < 500; ++j) {
        counter_15++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_15);
    std::thread t2(worker_15);
    t1.join();
    t2.join();
    std::cout << "Final counter_15 = " << counter_15 << std::endl;
    return 0;
}
