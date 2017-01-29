#include <iostream>
#include <thread>

class ThreadGuard {
public:
  explicit ThreadGuard(std::thread&& thread) :
    thread_(std::move(thread)) {
      if (!thread_.joinable()) {
        throw std::logic_error("Expected initialized object");
      }
    }
  ~ThreadGuard() {
    thread_.join();
  }
  ThreadGuard(const ThreadGuard& guard) = delete;
  ThreadGuard& operator=(const ThreadGuard& guard) = delete;
private:
  std::thread thread_;
};

void test() {
  std::cout << "hello" << std::endl;
}

int main() {
  std::thread mythread(test);
  ThreadGuard thread_guard(std::move(mythread));
  return 0;
}
