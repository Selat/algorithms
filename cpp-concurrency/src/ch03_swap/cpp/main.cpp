#include <mutex>
#include <iostream>
#include <list>
#include <thread>
#include <vector>

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
  void swap(MyList<T>& other) {
    // All-or-nothing semantics, just a simultaneous lock of all mutexes.
    std::lock(mutex_, other.mutex_);
    std::lock_guard<std::mutex> guard(mutex_, std::adopt_lock);
    std::lock_guard<std::mutex> other_guard(other.mutex_, std::adopt_lock);
    std::swap(list_, other.list_);
  }
private:
  std::mutex mutex_;
  std::list<T> list_;
};

std::mutex print_mutex;
void processList(int thread_id, MyList<int>& list1, MyList<int>& list2) {
  for (int i = 0; i < 10; ++i) {
    list1.PushBack(thread_id);
    list2.PushBack(thread_id);
    list1.swap(list2);
  }
  std::lock_guard<std::mutex> guard(print_mutex);
  std::cout << "L1: ";
  list1.Print();
  std::cout << "L2: ";
  list2.Print();
}

int main() {
  constexpr int kThreadsNum = 4;
  MyList<int> list1, list2;
  std::vector<std::thread> threads;
  for (int i = 0; i < kThreadsNum; ++i) {
    threads.push_back(std::thread(processList, i, std::ref(list1), std::ref(list2)));
  }

  for (auto& thread : threads) {
    thread.join();
  }
  return 0;
}
