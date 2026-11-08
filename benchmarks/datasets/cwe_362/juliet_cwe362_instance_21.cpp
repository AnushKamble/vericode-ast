// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 21)
#include <iostream>
#include <thread>
#include <vector>

int counter_21 = 0;

void worker_21() {
    for (int j = 0; j < 500; ++j) {
        counter_21++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_21);
    std::thread t2(worker_21);
    t1.join();
    t2.join();
    std::cout << "Final counter_21 = " << counter_21 << std::endl;
    return 0;
}
