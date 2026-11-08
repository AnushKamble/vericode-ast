// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 10)
#include <iostream>
#include <thread>
#include <vector>

int counter_10 = 0;

void worker_10() {
    for (int j = 0; j < 500; ++j) {
        counter_10++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_10);
    std::thread t2(worker_10);
    t1.join();
    t2.join();
    std::cout << "Final counter_10 = " << counter_10 << std::endl;
    return 0;
}
