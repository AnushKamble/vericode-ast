// Zero-Defect Control Baseline (Instance 02)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_02 = 0;
std::mutex mtx_02;

void safe_thread_02() {
    std::lock_guard<std::mutex> lock(mtx_02);
    safe_val_02 += 2;
}

void safe_buffer_02() {
    auto buf = std::make_unique<int[]>(22);
    buf[0] = 2;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_02();
    std::thread t(safe_thread_02);
    t.join();
    return 0;
}
