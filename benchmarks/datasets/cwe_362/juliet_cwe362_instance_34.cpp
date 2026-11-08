// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 34)
#include <iostream>
#include <thread>
#include <vector>

int counter_34 = 0;

void worker_34() {
    for (int j = 0; j < 500; ++j) {
        counter_34++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_34);
    std::thread t2(worker_34);
    t1.join();
    t2.join();
    std::cout << "Final counter_34 = " << counter_34 << std::endl;
    return 0;
}
