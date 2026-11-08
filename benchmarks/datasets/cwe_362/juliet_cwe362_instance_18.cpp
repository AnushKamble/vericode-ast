// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 18)
#include <iostream>
#include <thread>
#include <vector>

int counter_18 = 0;

void worker_18() {
    for (int j = 0; j < 500; ++j) {
        counter_18++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_18);
    std::thread t2(worker_18);
    t1.join();
    t2.join();
    std::cout << "Final counter_18 = " << counter_18 << std::endl;
    return 0;
}
