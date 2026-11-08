// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 08)
#include <iostream>
#include <thread>
#include <vector>

int counter_08 = 0;

void worker_08() {
    for (int j = 0; j < 500; ++j) {
        counter_08++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_08);
    std::thread t2(worker_08);
    t1.join();
    t2.join();
    std::cout << "Final counter_08 = " << counter_08 << std::endl;
    return 0;
}
