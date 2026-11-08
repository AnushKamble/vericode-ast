// NIST Juliet Test Suite v1.3 - CWE-362 Race Condition (Instance 11)
#include <iostream>
#include <thread>
#include <vector>

int counter_11 = 0;

void worker_11() {
    for (int j = 0; j < 500; ++j) {
        counter_11++; // Defect: unsynchronized concurrent shared mutation
    }
}

int main() {
    std::thread t1(worker_11);
    std::thread t2(worker_11);
    t1.join();
    t2.join();
    std::cout << "Final counter_11 = " << counter_11 << std::endl;
    return 0;
}
