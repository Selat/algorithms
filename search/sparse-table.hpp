#ifndef SPARSE_TABLE_HPP
#define SPARSE_TABLE_HPP

#include <vector>
#include <limits>
#include <cassert>
#include <cstdlib>
#include <set>

template <typename T>
std::vector<int> MergeStatistics(const std::vector<int>& v1, const std::vector<int>& v2,
                                 const std::vector<T>& data) {
  std::set<int> s;
  for (auto v : v1) {
    s.insert(v);
  }
  for (auto v : v2) {
    s.insert(v);
  }
  assert(s.size() >= 2);
  int min1_id = *s.begin();
  int min2_id = *(++s.begin());
  if (data[min1_id] > data[min2_id]) {
    std::swap(min1_id, min2_id);
  }
  for (auto id : s) {
    if (data[id] < data[min1_id]) {
      min2_id = min1_id;
      min1_id = id;
    } else if (data[id] < data[min2_id] && id != min1_id) {
      min2_id = id;
    }
  }
  assert(min1_id != min2_id);
  return {min1_id, min2_id};
}

// Idea: https://en.wikipedia.org/wiki/Range_minimum_query#Solution_using_constant_time_and_linearithmic_space
template <typename T>
class SparseTable {
 public:
  static SparseTable Init(const std::vector<T>& data) {
    SparseTable sparse_table;
    sparse_table.source_data_ = data;
    sparse_table.data_.resize(1);
    sparse_table.data_.push_back(std::vector<std::vector<int>>(data.size() - 1));
    for (size_t i = 0; i + 1 < data.size(); ++i) {
      int min_id = i;
      if (data[i + 1] < data[i]) {
        min_id = i + 1;
      }
      int max_id = i;
      if (data[i + 1] >= data[i]) {
        max_id = i + 1;
      }
      assert(min_id != max_id);
      sparse_table.data_.back()[i] = {min_id, max_id};
    }
    int curid = 1;
    for (int len = 4; len < data.size(); len <<= 1) {
      ++curid;
      sparse_table.data_.push_back(std::vector<std::vector<int>>(data.size() - len + 1));
      for (int i = 0; i + len <= data.size(); ++i) {
        const auto& v1 = sparse_table.data_[curid - 1][i];
        const auto& v2 = sparse_table.data_[curid - 1][i + len / 2];
        assert(v1.size() == 2);
        assert(v2.size() == 2);
        sparse_table.data_.back()[i] = MergeStatistics(v1, v2, data);
      }
    }
    return sparse_table;
  }
  std::vector<int> Query(int l, int r) {
    assert(r - 2 >= l);
    int id = 0;
    int tmp = r - l;
    while (tmp > 1) {
      ++id;
      tmp >>= 1;
    }
    if (r - l == (1 << id)) {
      return data_[id][l];
    } else {
      auto lid = data_[id][l];
      auto rid = data_[id][r - (1 << id)];
      assert(lid.size() == 2 && rid.size() == 2);
      return MergeStatistics(lid, rid, source_data_);
    }
  }
private:
  std::vector<T> source_data_;
  std::vector<std::vector<std::vector<int>>> data_;
};

#endif  // SPARSE_TABLE_HPP
