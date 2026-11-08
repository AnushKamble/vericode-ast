// Zero-Defect Control Baseline (Instance 07)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_07 = 0;
std::mutex mtx_07;

void safe_thread_07() {
    std::lock_guard<std::mutex> lock(mtx_07);
    safe_val_07 += 7;
}

void safe_buffer_07() {
    auto buf = std::make_unique<int[]>(27);
    buf[0] = 7;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_07();
    std::thread t(safe_thread_07);
    t.join();
    return 0;
}
