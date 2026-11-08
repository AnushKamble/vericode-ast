// Zero-Defect Control Baseline (Instance 12)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_12 = 0;
std::mutex mtx_12;

void safe_thread_12() {
    std::lock_guard<std::mutex> lock(mtx_12);
    safe_val_12 += 12;
}

void safe_buffer_12() {
    auto buf = std::make_unique<int[]>(32);
    buf[0] = 12;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_12();
    std::thread t(safe_thread_12);
    t.join();
    return 0;
}
