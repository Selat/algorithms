#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <vector>
#include <thread>

volatile int global_value = 0;

class Processor {
public:
  Processor(int value) : value_(value) {}
  void Run() {
    global_value += value_;
  }
private:
  int value_;
};

std::shared_ptr<Processor> processor;
std::once_flag processor_flag;

void InitProcessor() {
  processor.reset(new Processor(10));
}

void RunProcessor() {
  // Used for lazy initialization
  std::call_once(processor_flag, InitProcessor);
  processor->Run();
}

int main() {
  constexpr int kThreadsNum = 10;
  std::vector<std::thread> threads;

  for (int i = 0; i < kThreadsNum; ++i) {
    threads.push_back(std::thread(RunProcessor));
  }

  for (auto& thread : threads) {
    thread.join();
  }

  std::cout << global_value << std::endl;
  return 0;
}
