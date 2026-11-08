// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 16)
#include <iostream>
#include <thread>
#include <vector>

int counter_16 = 0;

void worker_16() {
    for (int j = 0; j < 500; ++j) {
        counter_16++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_16);
    std::thread t2(worker_16);
    t1.join();
    t2.join();
    std::cout << "Final counter_16 = " << counter_16 << std::endl;
    return 0;
}
