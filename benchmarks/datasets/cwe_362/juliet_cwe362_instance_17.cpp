// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 17)
#include <iostream>
#include <thread>
#include <vector>

int counter_17 = 0;

void worker_17() {
    for (int j = 0; j < 500; ++j) {
        counter_17++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_17);
    std::thread t2(worker_17);
    t1.join();
    t2.join();
    std::cout << "Final counter_17 = " << counter_17 << std::endl;
    return 0;
}
