// Zero-Defect Control Baseline (Instance 04)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_04 = 0;
std::mutex mtx_04;

void safe_thread_04() {
    std::lock_guard<std::mutex> lock(mtx_04);
    safe_val_04 += 4;
}

void safe_buffer_04() {
    auto buf = std::make_unique<int[]>(24);
    buf[0] = 4;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_04();
    std::thread t(safe_thread_04);
    t.join();
    return 0;
}
