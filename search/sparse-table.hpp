#ifndef SPARSE_TABLE_HPP
#define SPARSE_TABLE_HPP

#include <vector>
#include <limits>
#include <cassert>
#include <cstdlib>
#include <set>

template <typename T>
std::vector<size_t> MergeStatistics(const std::vector<size_t>& v, const std::vector<T>& data) {
  std::set<size_t> s;
  for (auto e : v) {
    s.insert(e);
  }
  assert(s.size() >= 2);
  size_t min1_id = *s.begin();
  size_t min2_id = *(++s.begin());
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
    sparse_table.data_.push_back(std::vector<std::vector<size_t>>(data.size() - 1));
    for (size_t i = 0; i + 1 < data.size(); ++i) {
      size_t min_id = i;
      if (data[i + 1] < data[i]) {
        min_id = i + 1;
      }
      size_t max_id = i;
      if (data[i + 1] >= data[i]) {
        max_id = i + 1;
      }
      assert(min_id != max_id);
      sparse_table.data_.back()[i] = {min_id, max_id};
    }
    int curid = 1;
    for (size_t len = 4; len < data.size(); len <<= 1) {
      ++curid;
      sparse_table.data_.push_back(std::vector<std::vector<size_t>>(data.size() - len + 1));
      for (int i = 0; i + len <= data.size(); ++i) {
        auto v1 = sparse_table.data_[curid - 1][i];
        const auto& v2 = sparse_table.data_[curid - 1][i + len / 2];
        assert(v1.size() == 2);
        assert(v2.size() == 2);
        v1.insert(v1.begin(), v2.begin(), v2.end());
        sparse_table.data_.back()[i] = MergeStatistics(v1, data);
      }
    }
    return sparse_table;
  }
  std::vector<size_t> Query(size_t l, size_t r) {
    assert(r > l);
    size_t id = 0;
    size_t tmp = r - l;
    while (tmp > 1) {
      ++id;
      tmp >>= 1;
    }
    if (r - l == 1) {
      return {l};
    } else {
      if (r == (1 << id) + l) {
        return data_[id][l];
      } else {
        auto lid = data_[id][l];
        const auto& rid = data_[id][r - (1 << id)];
        assert(lid.size() == 2 && rid.size() == 2);
        lid.insert(lid.end(), rid.begin(), rid.end());
        return MergeStatistics(lid, source_data_);
      }
    }
  }
private:
  std::vector<T> source_data_;
  std::vector<std::vector<std::vector<size_t>>> data_;
};

#endif  // SPARSE_TABLE_HPP
