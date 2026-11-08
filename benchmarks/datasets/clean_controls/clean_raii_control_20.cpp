// Zero-Defect Control Baseline (Instance 20)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_20 = 0;
std::mutex mtx_20;

void safe_thread_20() {
    std::lock_guard<std::mutex> lock(mtx_20);
    safe_val_20 += 20;
}

void safe_buffer_20() {
    auto buf = std::make_unique<int[]>(40);
    buf[0] = 20;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_20();
    std::thread t(safe_thread_20);
    t.join();
    return 0;
}
