// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 26)
#include <iostream>
#include <thread>
#include <vector>

int counter_26 = 0;

void worker_26() {
    for (int j = 0; j < 500; ++j) {
        counter_26++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_26);
    std::thread t2(worker_26);
    t1.join();
    t2.join();
    std::cout << "Final counter_26 = " << counter_26 << std::endl;
    return 0;
}
