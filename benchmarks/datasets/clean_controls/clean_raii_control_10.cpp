// Zero-Defect Control Baseline (Instance 10)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_10 = 0;
std::mutex mtx_10;

void safe_thread_10() {
    std::lock_guard<std::mutex> lock(mtx_10);
    safe_val_10 += 10;
}

void safe_buffer_10() {
    auto buf = std::make_unique<int[]>(30);
    buf[0] = 10;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_10();
    std::thread t(safe_thread_10);
    t.join();
    return 0;
}
