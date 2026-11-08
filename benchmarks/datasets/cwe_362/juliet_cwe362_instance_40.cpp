// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 40)
#include <iostream>
#include <thread>
#include <vector>

int counter_40 = 0;

void worker_40() {
    for (int j = 0; j < 500; ++j) {
        counter_40++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_40);
    std::thread t2(worker_40);
    t1.join();
    t2.join();
    std::cout << "Final counter_40 = " << counter_40 << std::endl;
    return 0;
}
