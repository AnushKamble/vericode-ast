// Zero-Defect Control Baseline (Instance 14)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_14 = 0;
std::mutex mtx_14;

void safe_thread_14() {
    std::lock_guard<std::mutex> lock(mtx_14);
    safe_val_14 += 14;
}

void safe_buffer_14() {
    auto buf = std::make_unique<int[]>(34);
    buf[0] = 14;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_14();
    std::thread t(safe_thread_14);
    t.join();
    return 0;
}
