// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 07)
#include <iostream>
#include <thread>
#include <vector>

int counter_07 = 0;

void worker_07() {
    for (int j = 0; j < 500; ++j) {
        counter_07++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_07);
    std::thread t2(worker_07);
    t1.join();
    t2.join();
    std::cout << "Final counter_07 = " << counter_07 << std::endl;
    return 0;
}
