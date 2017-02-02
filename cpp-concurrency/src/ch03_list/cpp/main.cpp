#include <iostream>
#include <list>
#include <mutex>
#include <vector>
#include <thread>

template <typename T>
class MyList {
public:
  void Print() {
    std::lock_guard<std::mutex> guard(mutex_);
    for (const auto& e : list_) {
      std::cout << e << " ";
    }
    std::cout << std::endl;
  }
  void PushFront(const T& val) {
    std::lock_guard<std::mutex> guard(mutex_);
    list_.push_front(val);
  }
  void PushBack(const T& val) {
    std::lock_guard<std::mutex> guard(mutex_);
    list_.push_back(val);
  }
private:
  std::mutex mutex_;
  std::list<T> list_;
};

void modifyList(MyList<int>& list) {
  for (int i = 0; i < 5; ++i) {
    list.PushFront(rand() % 20);
    list.PushBack(rand() % 20);
    list.Print();
  }
}

int main() {
  MyList<int> list;
  std::vector<std::thread> threads;
  constexpr int kThreadsNum = 5;
  for (int i = 0; i < kThreadsNum; ++i) {
    threads.push_back(std::thread(modifyList, std::ref(list)));
  }
  for (auto& thread : threads) {
    thread.join();
  }
  return 0;
}
