// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 31)
#include <iostream>
#include <thread>
#include <vector>

int counter_31 = 0;

void worker_31() {
    for (int j = 0; j < 500; ++j) {
        counter_31++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_31);
    std::thread t2(worker_31);
    t1.join();
    t2.join();
    std::cout << "Final counter_31 = " << counter_31 << std::endl;
    return 0;
}
