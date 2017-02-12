#include <iostream>
#include <thread>
#include <mutex>

bool is_finished = false;
std::mutex is_finished_mutex;

void Compute() {
  for (volatile int i = 0; i < 1000 * 1000 * 1000; ++i);
  std::lock_guard<std::mutex> lock(is_finished_mutex);
  is_finished = true;
}

void WaitForCompute() {
  std::unique_lock<std::mutex> lock(is_finished_mutex);
  int checks_num = 0;
  while (!is_finished) {
    lock.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ++checks_num;
    lock.lock();
  }
  std::cout << "Computation is finished after " << checks_num << " checks" << std::endl;
}

int main() {
  std::thread thread1(Compute);
  std::thread thread2(WaitForCompute);
  thread2.join();
  std::cout << "Thread 2 has stopped" << std::endl;
  thread1.join();
  return 0;
}
