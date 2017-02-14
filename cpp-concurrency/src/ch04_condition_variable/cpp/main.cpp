#include <iostream>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <functional>
#include <thread>

template <typename T, typename F, typename E>
class Processor {
public:
  Processor(F processor, E end_cond)
      : processor_(processor), end_cond_(end_cond) {}

  void AddTask(T parameter) {
    std::lock_guard<std::mutex> guard(mutex_);
    tasks_.push(parameter);
    tasks_cond_.notify_one();
  }

  void Run() {
    while (true) {
      std::unique_lock<std::mutex> lock(mutex_);
      tasks_cond_.wait(lock, [this]{return !tasks_.empty();});
      T parameter = tasks_.front();
      tasks_.pop();
      lock.unlock();
      if (end_cond_(parameter)) {
        break;
      }
      processor_(parameter);
    }
  }
private:
  std::condition_variable tasks_cond_;
  std::mutex mutex_;
  std::queue<T> tasks_;
  F processor_;
  E end_cond_;
};

int main() {
  typedef Processor<int, std::function<void(int)>, std::function<bool(int)>>
      IntProcessor;
  int sum;
  auto sumf = [&sum](int a) {sum += a;};
  auto zerop = [](int a) {return a == 0;};
  IntProcessor processor(sumf, zerop);
  std::thread proc_thread(std::bind(&IntProcessor::Run, &processor));
  std::thread feed_thread([&processor]() {
    for (int i = 1; i <= 1000; ++i) {
      processor.AddTask(i);
    }
    processor.AddTask(0);
  });
  proc_thread.join();
  feed_thread.join();
  std::cout << sum << std::endl;
  return 0;
}
