// Zero-Defect Control Baseline (Instance 21)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_21 = 0;
std::mutex mtx_21;

void safe_thread_21() {
    std::lock_guard<std::mutex> lock(mtx_21);
    safe_val_21 += 21;
}

void safe_buffer_21() {
    auto buf = std::make_unique<int[]>(41);
    buf[0] = 21;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_21();
    std::thread t(safe_thread_21);
    t.join();
    return 0;
}
