// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 29)
#include <iostream>
#include <thread>
#include <vector>

int counter_29 = 0;

void worker_29() {
    for (int j = 0; j < 500; ++j) {
        counter_29++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_29);
    std::thread t2(worker_29);
    t1.join();
    t2.join();
    std::cout << "Final counter_29 = " << counter_29 << std::endl;
    return 0;
}
