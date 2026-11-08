// Zero-Defect Control Baseline (Instance 05)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_05 = 0;
std::mutex mtx_05;

void safe_thread_05() {
    std::lock_guard<std::mutex> lock(mtx_05);
    safe_val_05 += 5;
}

void safe_buffer_05() {
    auto buf = std::make_unique<int[]>(25);
    buf[0] = 5;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_05();
    std::thread t(safe_thread_05);
    t.join();
    return 0;
}
