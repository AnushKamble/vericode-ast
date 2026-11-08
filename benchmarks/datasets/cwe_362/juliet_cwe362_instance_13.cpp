// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 13)
#include <iostream>
#include <thread>
#include <vector>

int counter_13 = 0;

void worker_13() {
    for (int j = 0; j < 500; ++j) {
        counter_13++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_13);
    std::thread t2(worker_13);
    t1.join();
    t2.join();
    std::cout << "Final counter_13 = " << counter_13 << std::endl;
    return 0;
}
