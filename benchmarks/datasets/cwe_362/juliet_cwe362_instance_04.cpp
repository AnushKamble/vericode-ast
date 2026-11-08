// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 04)
#include <iostream>
#include <thread>
#include <vector>

int counter_04 = 0;

void worker_04() {
    for (int j = 0; j < 500; ++j) {
        counter_04++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_04);
    std::thread t2(worker_04);
    t1.join();
    t2.join();
    std::cout << "Final counter_04 = " << counter_04 << std::endl;
    return 0;
}
