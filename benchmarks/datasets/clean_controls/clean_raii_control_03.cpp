// Zero-Defect Control Baseline (Instance 03)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_03 = 0;
std::mutex mtx_03;

void safe_thread_03() {
    std::lock_guard<std::mutex> lock(mtx_03);
    safe_val_03 += 3;
}

void safe_buffer_03() {
    auto buf = std::make_unique<int[]>(23);
    buf[0] = 3;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_03();
    std::thread t(safe_thread_03);
    t.join();
    return 0;
}
