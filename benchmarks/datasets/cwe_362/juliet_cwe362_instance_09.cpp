// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 09)
#include <iostream>
#include <thread>
#include <vector>

int counter_09 = 0;

void worker_09() {
    for (int j = 0; j < 500; ++j) {
        counter_09++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_09);
    std::thread t2(worker_09);
    t1.join();
    t2.join();
    std::cout << "Final counter_09 = " << counter_09 << std::endl;
    return 0;
}
