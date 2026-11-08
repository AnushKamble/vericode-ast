// Zero-Defect Control Baseline (Instance 28)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_28 = 0;
std::mutex mtx_28;

void safe_thread_28() {
    std::lock_guard<std::mutex> lock(mtx_28);
    safe_val_28 += 28;
}

void safe_buffer_28() {
    auto buf = std::make_unique<int[]>(48);
    buf[0] = 28;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_28();
    std::thread t(safe_thread_28);
    t.join();
    return 0;
}
