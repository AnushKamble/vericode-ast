// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 33)
#include <iostream>
#include <thread>
#include <vector>

int counter_33 = 0;

void worker_33() {
    for (int j = 0; j < 500; ++j) {
        counter_33++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_33);
    std::thread t2(worker_33);
    t1.join();
    t2.join();
    std::cout << "Final counter_33 = " << counter_33 << std::endl;
    return 0;
}
