// Zero-Defect Control Baseline (Instance 19)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_19 = 0;
std::mutex mtx_19;

void safe_thread_19() {
    std::lock_guard<std::mutex> lock(mtx_19);
    safe_val_19 += 19;
}

void safe_buffer_19() {
    auto buf = std::make_unique<int[]>(39);
    buf[0] = 19;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_19();
    std::thread t(safe_thread_19);
    t.join();
    return 0;
}
