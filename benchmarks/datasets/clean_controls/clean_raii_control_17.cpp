// Zero-Defect Control Baseline (Instance 17)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_17 = 0;
std::mutex mtx_17;

void safe_thread_17() {
    std::lock_guard<std::mutex> lock(mtx_17);
    safe_val_17 += 17;
}

void safe_buffer_17() {
    auto buf = std::make_unique<int[]>(37);
    buf[0] = 17;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_17();
    std::thread t(safe_thread_17);
    t.join();
    return 0;
}
