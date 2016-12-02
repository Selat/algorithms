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
    sparse_table.source_data_ = data;
    for (int len = 1; len < data.size(); len <<= 1) {
      sparse_table.data_.push_back(std::vector<int>(data.size() - len + 1));
      QueueWithMaximum<std::pair<int, int>> q;
      for (int i = 0; i < len; ++i) {
        q.PushBack(std::make_pair(-data[i], i));
      }
      for (int i = 0; i + len < data.size(); ++i) {
        sparse_table.data_.back()[i] = q.PeekMax().second;
        q.PopFront();
        q.PushBack(std::make_pair(-data[i + len], i + len));
      }
      sparse_table.data_.back()[data.size() - len] = q.PeekMax().second;
    }
    return sparse_table;
  }
  int Query(int l, int r) {
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
      int lid = data_[id][l];
      int rid = data_[id][r - (1 << id)];
      if (source_data_[lid] < source_data_[rid]) {
        return lid;
      } else {
        return rid;
      }
    }
  }
private:
  SparseTable() {
  }
  std::vector<T> source_data_;
  std::vector<std::vector<int>> data_;
};

#endif  // SPARSE_TABLE_HPP
