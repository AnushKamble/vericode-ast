// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 39)
#include <iostream>
#include <thread>
#include <vector>

int counter_39 = 0;

void worker_39() {
    for (int j = 0; j < 500; ++j) {
        counter_39++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_39);
    std::thread t2(worker_39);
    t1.join();
    t2.join();
    std::cout << "Final counter_39 = " << counter_39 << std::endl;
    return 0;
}
