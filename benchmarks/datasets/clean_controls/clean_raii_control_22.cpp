// Zero-Defect Control Baseline (Instance 22)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_22 = 0;
std::mutex mtx_22;

void safe_thread_22() {
    std::lock_guard<std::mutex> lock(mtx_22);
    safe_val_22 += 22;
}

void safe_buffer_22() {
    auto buf = std::make_unique<int[]>(42);
    buf[0] = 22;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_22();
    std::thread t(safe_thread_22);
    t.join();
    return 0;
}
