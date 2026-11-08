// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 25)
#include <iostream>
#include <thread>
#include <vector>

int counter_25 = 0;

void worker_25() {
    for (int j = 0; j < 500; ++j) {
        counter_25++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_25);
    std::thread t2(worker_25);
    t1.join();
    t2.join();
    std::cout << "Final counter_25 = " << counter_25 << std::endl;
    return 0;
}
