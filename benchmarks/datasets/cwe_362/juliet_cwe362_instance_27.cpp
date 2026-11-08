// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 27)
#include <iostream>
#include <thread>
#include <vector>

int counter_27 = 0;

void worker_27() {
    for (int j = 0; j < 500; ++j) {
        counter_27++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_27);
    std::thread t2(worker_27);
    t1.join();
    t2.join();
    std::cout << "Final counter_27 = " << counter_27 << std::endl;
    return 0;
}
