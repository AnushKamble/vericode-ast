// Zero-Defect Control Baseline (Instance 30)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_30 = 0;
std::mutex mtx_30;

void safe_thread_30() {
    std::lock_guard<std::mutex> lock(mtx_30);
    safe_val_30 += 30;
}

void safe_buffer_30() {
    auto buf = std::make_unique<int[]>(50);
    buf[0] = 30;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_30();
    std::thread t(safe_thread_30);
    t.join();
    return 0;
}
