// Zero-Defect Control Baseline (Instance 26)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_26 = 0;
std::mutex mtx_26;

void safe_thread_26() {
    std::lock_guard<std::mutex> lock(mtx_26);
    safe_val_26 += 26;
}

void safe_buffer_26() {
    auto buf = std::make_unique<int[]>(46);
    buf[0] = 26;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_26();
    std::thread t(safe_thread_26);
    t.join();
    return 0;
}
