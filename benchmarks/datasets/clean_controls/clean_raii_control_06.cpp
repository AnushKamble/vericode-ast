// Zero-Defect Control Baseline (Instance 06)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_06 = 0;
std::mutex mtx_06;

void safe_thread_06() {
    std::lock_guard<std::mutex> lock(mtx_06);
    safe_val_06 += 6;
}

void safe_buffer_06() {
    auto buf = std::make_unique<int[]>(26);
    buf[0] = 6;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_06();
    std::thread t(safe_thread_06);
    t.join();
    return 0;
}
