// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 24)
#include <iostream>
#include <thread>
#include <vector>

int counter_24 = 0;

void worker_24() {
    for (int j = 0; j < 500; ++j) {
        counter_24++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_24);
    std::thread t2(worker_24);
    t1.join();
    t2.join();
    std::cout << "Final counter_24 = " << counter_24 << std::endl;
    return 0;
}
