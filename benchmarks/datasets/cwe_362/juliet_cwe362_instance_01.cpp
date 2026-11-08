// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 01)
#include <iostream>
#include <thread>
#include <vector>

int counter_01 = 0;

void worker_01() {
    for (int j = 0; j < 500; ++j) {
        counter_01++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_01);
    std::thread t2(worker_01);
    t1.join();
    t2.join();
    std::cout << "Final counter_01 = " << counter_01 << std::endl;
    return 0;
}
