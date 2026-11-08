// Zero-Defect Control Baseline (Instance 13)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_13 = 0;
std::mutex mtx_13;

void safe_thread_13() {
    std::lock_guard<std::mutex> lock(mtx_13);
    safe_val_13 += 13;
}

void safe_buffer_13() {
    auto buf = std::make_unique<int[]>(33);
    buf[0] = 13;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_13();
    std::thread t(safe_thread_13);
    t.join();
    return 0;
}
