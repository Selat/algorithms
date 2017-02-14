#include <iostream>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <functional>
#include <thread>

template <typename T>
class MyQueue {
public:
  MyQueue() {}

  bool IsEmpty() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return data_.empty();
  }

  void Push(T value) {
    std::lock_guard<std::mutex> guard(mutex_);
    data_.push(value);
    push_signal_.notify_one();
  }

  bool TryPop(T& value) {
    std::lock_guard<std::mutex> guard(mutex_);
    if (data_.empty()) {
      return false;
    } else {
      value = data_.front();
      data_.pop();
      return true;
    }
  }

  std::shared_ptr<T> TryPop() {
    std::lock_guard<std::mutex> guard(mutex_);
    if (data_.empty()) {
      return nullptr;
    } else {
      const auto res = std::make_shared(data_.front());
      data_.pop();
      return res;
    }
  }

  void Pop(T& value) {
    std::unique_lock<std::mutex> lock(mutex_);
    push_signal_.wait(lock, [this]{return !data_.empty();});
    value = data_.front();
    data_.pop();
  }

  std::shared_ptr<T> Pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    push_signal_.wait(lock, [this]{return !data_.empty();});
    const auto res = std::make_shared<T>(data_.front());
    data_.pop();
    return res;
  }
private:
  std::queue<T> data_;
  std::condition_variable push_signal_;
  mutable std::mutex mutex_;
};

template <typename T, typename F, typename E>
class Processor {
public:
  Processor(F processor, E end_cond)
      : processor_(processor), end_cond_(end_cond) {}

  void AddTask(T parameter) {
    tasks_.Push(parameter);
  }

  void Run() {
    while (true) {
      T parameter = *tasks_.Pop();
      if (end_cond_(parameter)) {
        break;
      }
      processor_(parameter);
    }
  }
private:
  MyQueue<T> tasks_;
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
