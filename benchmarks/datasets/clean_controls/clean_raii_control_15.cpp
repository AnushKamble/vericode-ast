// Zero-Defect Control Baseline (Instance 15)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_15 = 0;
std::mutex mtx_15;

void safe_thread_15() {
    std::lock_guard<std::mutex> lock(mtx_15);
    safe_val_15 += 15;
}

void safe_buffer_15() {
    auto buf = std::make_unique<int[]>(35);
    buf[0] = 15;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_15();
    std::thread t(safe_thread_15);
    t.join();
    return 0;
}
