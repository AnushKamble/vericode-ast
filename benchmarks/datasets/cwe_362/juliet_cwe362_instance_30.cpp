// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 30)
#include <iostream>
#include <thread>
#include <vector>

int counter_30 = 0;

void worker_30() {
    for (int j = 0; j < 500; ++j) {
        counter_30++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_30);
    std::thread t2(worker_30);
    t1.join();
    t2.join();
    std::cout << "Final counter_30 = " << counter_30 << std::endl;
    return 0;
}
