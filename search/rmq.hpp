#ifndef RMQ_HPP
#define RMQ_HPP

#include <vector>
#include <limits>
#include <cassert>

#include "max-queue.hpp"

template <typename T>
class Rmq {
 public:
  static Rmq Init(const std::vector<T>& data) {
    Rmq rmq;
    for (int len = 1; len < data.size(); len <<= 1) {
      rmq.data_.push_back(std::vector<int>(data.size() - len + 1));
      QueueWithMaximum<int> q;
      for (int i = 0; i < len; ++i) {
        q.PushBack(-data[i]);
      }
      for (int i = 0; i + len < data.size(); ++i) {
        rmq.data_.back()[i] = -q.PeekMax();
        q.PopFront();
        q.PushBack(-data[i + len]);
      }
      rmq.data_.back()[data.size() - len] = -q.PeekMax();
    }
    return rmq;
  }
  T Query(int l, int r) {
    assert(r > l);
    if (r - l == 1) {
      return data_[0][l];
    } else {
      int id = 0;
      int tmp = r - l;
      while (tmp > 1) {
        ++id;
        tmp >>= 1;
      }
      int lid = l;
      int rid = r - (1 << id);
      return std::min(data_[id][lid], data_[id][rid]);
    }
  }
private:
  Rmq() {
  }
  std::vector<std::vector<T>> data_;
};

#endif  // RMQ_HPP
