#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

void slowFunction(int id) {
  for (volatile int i = 0; i < 1000 * 1000 * 100; ++i);
  std::cout << "thread #" << id << std::endl;
}

int main() {
  std::cout << std::thread::hardware_concurrency() << std::endl;
  const int kThreadsNum = std::thread::hardware_concurrency();
  std::vector<std::thread> threads;
  for (int i = 0; i < kThreadsNum; ++i) {
    threads.push_back(std::thread(slowFunction, i));
  }
  std::for_each(threads.begin(), threads.end(),
                std::mem_fn(&std::thread::join));
  return 0;
}
