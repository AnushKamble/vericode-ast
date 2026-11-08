// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 28)
#include <iostream>
#include <thread>
#include <vector>

int counter_28 = 0;

void worker_28() {
    for (int j = 0; j < 500; ++j) {
        counter_28++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_28);
    std::thread t2(worker_28);
    t1.join();
    t2.join();
    std::cout << "Final counter_28 = " << counter_28 << std::endl;
    return 0;
}
