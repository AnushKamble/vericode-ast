// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 05)
#include <iostream>
#include <thread>
#include <vector>

int counter_05 = 0;

void worker_05() {
    for (int j = 0; j < 500; ++j) {
        counter_05++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_05);
    std::thread t2(worker_05);
    t1.join();
    t2.join();
    std::cout << "Final counter_05 = " << counter_05 << std::endl;
    return 0;
}
