#ifndef SPARSE_TABLE_HPP
#define SPARSE_TABLE_HPP

#include <vector>
#include <limits>
#include <cassert>

#include "max-queue.hpp"

// Idea: https://en.wikipedia.org/wiki/Range_minimum_query#Solution_using_constant_time_and_linearithmic_space
template <typename T>
class SparseTable {
 public:
  static SparseTable Init(const std::vector<T>& data) {
    SparseTable sparse_table;
    for (int len = 1; len < data.size(); len <<= 1) {
      sparse_table.data_.push_back(std::vector<int>(data.size() - len + 1));
      QueueWithMaximum<int> q;
      for (int i = 0; i < len; ++i) {
        q.PushBack(-data[i]);
      }
      for (int i = 0; i + len < data.size(); ++i) {
        sparse_table.data_.back()[i] = -q.PeekMax();
        q.PopFront();
        q.PushBack(-data[i + len]);
      }
      sparse_table.data_.back()[data.size() - len] = -q.PeekMax();
    }
    return sparse_table;
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
  SparseTable() {
  }
  std::vector<std::vector<T>> data_;
};

#endif  // SPARSE_TABLE_HPP
