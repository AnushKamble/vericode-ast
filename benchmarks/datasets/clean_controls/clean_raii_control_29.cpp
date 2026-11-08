// Zero-Defect Control Baseline (Instance 29)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_29 = 0;
std::mutex mtx_29;

void safe_thread_29() {
    std::lock_guard<std::mutex> lock(mtx_29);
    safe_val_29 += 29;
}

void safe_buffer_29() {
    auto buf = std::make_unique<int[]>(49);
    buf[0] = 29;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_29();
    std::thread t(safe_thread_29);
    t.join();
    return 0;
}
