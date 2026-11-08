// Zero-Defect Control Baseline (Instance 01)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_01 = 0;
std::mutex mtx_01;

void safe_thread_01() {
    std::lock_guard<std::mutex> lock(mtx_01);
    safe_val_01 += 1;
}

void safe_buffer_01() {
    auto buf = std::make_unique<int[]>(21);
    buf[0] = 1;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_01();
    std::thread t(safe_thread_01);
    t.join();
    return 0;
}
