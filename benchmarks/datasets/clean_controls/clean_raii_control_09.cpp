// Zero-Defect Control Baseline (Instance 09)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_09 = 0;
std::mutex mtx_09;

void safe_thread_09() {
    std::lock_guard<std::mutex> lock(mtx_09);
    safe_val_09 += 9;
}

void safe_buffer_09() {
    auto buf = std::make_unique<int[]>(29);
    buf[0] = 9;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_09();
    std::thread t(safe_thread_09);
    t.join();
    return 0;
}
