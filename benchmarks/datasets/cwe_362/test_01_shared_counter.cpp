// Test Case: CWE-362 Race Condition on Unsynchronized Shared Variable
#include <iostream>
#include <thread>
#include <vector>

int counter = 0;

void worker() {
    for (int i = 0; i < 1000; ++i) {
        counter++; // Defect: Unsynchronized concurrent modification
    }
}

int main() {
    std::thread t1(worker);
    std::thread t2(worker);
    t1.join();
    t2.join();
    std::cout << "Final counter: " << counter << std::endl;
    return 0;
}
