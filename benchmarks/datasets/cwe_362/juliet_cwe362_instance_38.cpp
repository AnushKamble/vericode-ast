// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 38)
#include <iostream>
#include <thread>
#include <vector>

int counter_38 = 0;

void worker_38() {
    for (int j = 0; j < 500; ++j) {
        counter_38++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_38);
    std::thread t2(worker_38);
    t1.join();
    t2.join();
    std::cout << "Final counter_38 = " << counter_38 << std::endl;
    return 0;
}
