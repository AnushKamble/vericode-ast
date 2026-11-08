// Test Case: Clean Thread-Safe Mutex Guard (Zero Defect Baseline)
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

int counter = 0;
std::mutex mtx;

void safe_worker() {
    for (int i = 0; i < 1000; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
        counter++;
    }
}

int main() {
    std::thread t1(safe_worker);
    std::thread t2(safe_worker);
    t1.join();
    t2.join();
    std::cout << "Thread-safe counter: " << counter << std::endl;
    return 0;
}
