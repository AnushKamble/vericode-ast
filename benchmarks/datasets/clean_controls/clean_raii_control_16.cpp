// Zero-Defect Control Baseline (Instance 16)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_16 = 0;
std::mutex mtx_16;

void safe_thread_16() {
    std::lock_guard<std::mutex> lock(mtx_16);
    safe_val_16 += 16;
}

void safe_buffer_16() {
    auto buf = std::make_unique<int[]>(36);
    buf[0] = 16;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_16();
    std::thread t(safe_thread_16);
    t.join();
    return 0;
}
