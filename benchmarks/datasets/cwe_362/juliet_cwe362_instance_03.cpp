// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 03)
#include <iostream>
#include <thread>
#include <vector>

int counter_03 = 0;

void worker_03() {
    for (int j = 0; j < 500; ++j) {
        counter_03++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_03);
    std::thread t2(worker_03);
    t1.join();
    t2.join();
    std::cout << "Final counter_03 = " << counter_03 << std::endl;
    return 0;
}
