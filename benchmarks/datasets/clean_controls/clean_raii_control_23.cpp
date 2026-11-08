// Zero-Defect Control Baseline (Instance 23)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_23 = 0;
std::mutex mtx_23;

void safe_thread_23() {
    std::lock_guard<std::mutex> lock(mtx_23);
    safe_val_23 += 23;
}

void safe_buffer_23() {
    auto buf = std::make_unique<int[]>(43);
    buf[0] = 23;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_23();
    std::thread t(safe_thread_23);
    t.join();
    return 0;
}
