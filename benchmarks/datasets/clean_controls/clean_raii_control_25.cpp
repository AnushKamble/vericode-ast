// Zero-Defect Control Baseline (Instance 25)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_25 = 0;
std::mutex mtx_25;

void safe_thread_25() {
    std::lock_guard<std::mutex> lock(mtx_25);
    safe_val_25 += 25;
}

void safe_buffer_25() {
    auto buf = std::make_unique<int[]>(45);
    buf[0] = 25;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_25();
    std::thread t(safe_thread_25);
    t.join();
    return 0;
}
