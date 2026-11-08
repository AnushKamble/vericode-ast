// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 20)
#include <iostream>
#include <thread>
#include <vector>

int counter_20 = 0;

void worker_20() {
    for (int j = 0; j < 500; ++j) {
        counter_20++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_20);
    std::thread t2(worker_20);
    t1.join();
    t2.join();
    std::cout << "Final counter_20 = " << counter_20 << std::endl;
    return 0;
}
