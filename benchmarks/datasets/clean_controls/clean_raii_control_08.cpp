// Zero-Defect Control Baseline (Instance 08)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_08 = 0;
std::mutex mtx_08;

void safe_thread_08() {
    std::lock_guard<std::mutex> lock(mtx_08);
    safe_val_08 += 8;
}

void safe_buffer_08() {
    auto buf = std::make_unique<int[]>(28);
    buf[0] = 8;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_08();
    std::thread t(safe_thread_08);
    t.join();
    return 0;
}
