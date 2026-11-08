// Zero-Defect Control Baseline (Instance 27)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_27 = 0;
std::mutex mtx_27;

void safe_thread_27() {
    std::lock_guard<std::mutex> lock(mtx_27);
    safe_val_27 += 27;
}

void safe_buffer_27() {
    auto buf = std::make_unique<int[]>(47);
    buf[0] = 27;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_27();
    std::thread t(safe_thread_27);
    t.join();
    return 0;
}
