// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 35)
#include <iostream>
#include <thread>
#include <vector>

int counter_35 = 0;

void worker_35() {
    for (int j = 0; j < 500; ++j) {
        counter_35++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_35);
    std::thread t2(worker_35);
    t1.join();
    t2.join();
    std::cout << "Final counter_35 = " << counter_35 << std::endl;
    return 0;
}
