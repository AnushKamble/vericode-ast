// Zero-Defect Control Baseline (Instance 18)
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

int safe_val_18 = 0;
std::mutex mtx_18;

void safe_thread_18() {
    std::lock_guard<std::mutex> lock(mtx_18);
    safe_val_18 += 18;
}

void safe_buffer_18() {
    auto buf = std::make_unique<int[]>(38);
    buf[0] = 18;
    std::cout << "Safe RAII buffer initialized: " << buf[0] << std::endl;
}

int main() {
    safe_buffer_18();
    std::thread t(safe_thread_18);
    t.join();
    return 0;
}
