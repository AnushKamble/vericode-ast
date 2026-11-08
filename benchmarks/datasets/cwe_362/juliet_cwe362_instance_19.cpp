// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 19)
#include <iostream>
#include <thread>
#include <vector>

int counter_19 = 0;

void worker_19() {
    for (int j = 0; j < 500; ++j) {
        counter_19++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_19);
    std::thread t2(worker_19);
    t1.join();
    t2.join();
    std::cout << "Final counter_19 = " << counter_19 << std::endl;
    return 0;
}
