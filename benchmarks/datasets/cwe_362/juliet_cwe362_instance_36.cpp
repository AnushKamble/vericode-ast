// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 36)
#include <iostream>
#include <thread>
#include <vector>

int counter_36 = 0;

void worker_36() {
    for (int j = 0; j < 500; ++j) {
        counter_36++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_36);
    std::thread t2(worker_36);
    t1.join();
    t2.join();
    std::cout << "Final counter_36 = " << counter_36 << std::endl;
    return 0;
}
