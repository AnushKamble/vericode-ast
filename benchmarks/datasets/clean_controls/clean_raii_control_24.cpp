// Zero-Defect Control Baseline (Instance 24)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_24 = 0;
std::mutex mtx_24;

void safe_thread_24() {
    std::lock_guard<std::mutex> lock(mtx_24);
    safe_val_24 += 24;
}

void safe_buffer_24() {
    auto buf = std::make_unique<int[]>(44);
    buf[0] = 24;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_24();
    std::thread t(safe_thread_24);
    t.join();
    return 0;
}
