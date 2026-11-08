// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 23)
#include <iostream>
#include <thread>
#include <vector>

int counter_23 = 0;

void worker_23() {
    for (int j = 0; j < 500; ++j) {
        counter_23++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_23);
    std::thread t2(worker_23);
    t1.join();
    t2.join();
    std::cout << "Final counter_23 = " << counter_23 << std::endl;
    return 0;
}
