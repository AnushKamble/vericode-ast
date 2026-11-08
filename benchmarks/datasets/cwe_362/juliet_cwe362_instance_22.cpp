// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 22)
#include <iostream>
#include <thread>
#include <vector>

int counter_22 = 0;

void worker_22() {
    for (int j = 0; j < 500; ++j) {
        counter_22++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_22);
    std::thread t2(worker_22);
    t1.join();
    t2.join();
    std::cout << "Final counter_22 = " << counter_22 << std::endl;
    return 0;
}
