#include <memory>
#include <stack>
#include <mutex>
#include <exception>
#include <vector>
#include <thread>
#include <iostream>

class StackEmpty : public std::exception {
public:
  StackEmpty() {}
  const char* what() const throw() {
    return "Pop() is called for an empty stack";
  }
};

// Thread-safe stack
template <typename T>
class MyStack {
public:
  MyStack() {}
  MyStack(const MyStack& other) {
    std::lock_guard<std::mutex> guard(mutex_);
    data_ = other.data_;
  }
  MyStack& operator=(const MyStack&) = delete;
  void Push(T value) {
    std::lock_guard<std::mutex> guard(mutex_);
    data_.push(value);
  }
  std::shared_ptr<T> Pop() {
    std::lock_guard<std::mutex> guard(mutex_);
    if (data_.empty()) {
      throw StackEmpty();
    }
    std::shared_ptr<T> res(std::make_shared<T>(data_.top()));
    data_.pop();
    return res;
  }
  void Pop(T& res) {
    std::lock_guard<std::mutex> guard(mutex_);
    if (data_.empty()) {
      throw StackEmpty();
    }
    res = data_.top();
    data_.pop();
  }
  bool IsEmpty() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return data_.empty();
  }
private:
  std::stack<T> data_;
  mutable std::mutex mutex_;
};

void processStack(MyStack<int>& s) {
  while (!s.IsEmpty()) {
    try {
      volatile int value = *s.Pop();
      value *= 10;
    } catch (StackEmpty& e) {
      std::cout << e.what() << std::endl;
      break;
    }
  }
}

int main() {
  MyStack<int> s;
  for (int i = 0; i < 100; ++i) {
    s.Push(i);
  }
  std::vector<std::thread> threads;
  for (int i = 0; i < 4; ++i) {
    threads.push_back(std::thread(processStack, std::ref(s)));
  }
  for (auto& thread : threads) {
    thread.join();
  }
  return 0;
}
