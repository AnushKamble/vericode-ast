// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 37)
#include <iostream>
#include <thread>
#include <vector>

int counter_37 = 0;

void worker_37() {
    for (int j = 0; j < 500; ++j) {
        counter_37++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_37);
    std::thread t2(worker_37);
    t1.join();
    t2.join();
    std::cout << "Final counter_37 = " << counter_37 << std::endl;
    return 0;
}
