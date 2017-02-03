#include <mutex>
#include <limits>
#include <thread>
#include <iostream>

class HierarchicalMutex {
 public:
  explicit HierarchicalMutex(int level)
      : level_(level), prev_level_(0) {
  }
  void lock() {
    std::cout << "lock " << level_ << std::endl;
    check_levels_invariant();
    mutex_.lock();
    update_levels_invariant();
  }
  bool try_lock() {
    check_levels_invariant();
    if (mutex_.try_lock()) {
      update_levels_invariant();
      return true;
    } else {
      return false;
    }
  }
  void unlock() {
    std::cout << "unlock " << level_ << std::endl;
    cur_level_ = prev_level_;
    mutex_.unlock();
  }
 private:
  void check_levels_invariant() {
    if (level_ >= cur_level_) {
      throw std::logic_error("HierarchicalMutex levels invariant is broken");
    }
  }
  void update_levels_invariant() {
    prev_level_ = cur_level_;
    cur_level_ = level_;
  }
  unsigned long prev_level_;
  unsigned long level_;
  std::mutex mutex_;
  static thread_local unsigned long cur_level_;
};

thread_local unsigned long HierarchicalMutex::cur_level_(
    std::numeric_limits<unsigned long>::max());

int main() {
  HierarchicalMutex mutex1(4);
  HierarchicalMutex mutex2(5);
  std::lock_guard<HierarchicalMutex> guard1(mutex1);
  std::lock_guard<HierarchicalMutex> guard2(mutex2);
  return 0;
}
