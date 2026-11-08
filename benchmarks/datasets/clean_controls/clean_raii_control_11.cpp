// Zero-Defect Control Baseline (Instance 11)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_11 = 0;
std::mutex mtx_11;

void safe_thread_11() {
    std::lock_guard<std::mutex> lock(mtx_11);
    safe_val_11 += 11;
}

void safe_buffer_11() {
    auto buf = std::make_unique<int[]>(31);
    buf[0] = 11;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_11();
    std::thread t(safe_thread_11);
    t.join();
    return 0;
}
