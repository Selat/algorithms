#ifndef SPARSE_TABLE_HPP
#define SPARSE_TABLE_HPP

#include <vector>
#include <limits>
#include <cassert>
#include <cstdlib>
#include <set>

// Idea: https://en.wikipedia.org/wiki/Range_minimum_query#Solution_using_constant_time_and_linearithmic_space
template <typename T>
class SparseTable {
 public:
  SparseTable() {}
  void Init(const std::vector<T>& data);
  size_t QueryMin(size_t l, size_t r, const std::vector<T>& data);
  std::vector<size_t> Query(size_t l, size_t r, const std::vector<T>& data);
 private:
  std::vector<std::vector<int>> data_;
};

template <typename T>
void SparseTable<T>::Init(const std::vector<T>& data) {
  data_.resize(1);
  int curid = 0;
  for (size_t len = 2; len < data.size(); len <<= 1) {
    ++curid;
    data_.push_back(std::vector<int>(data.size() - len + 1));
    for (int i = 0; i + len <= data.size(); ++i) {
      size_t id1, id2;
      if (curid == 1) {
        id1 = i;
        id2 = i + 1;
      } else {
        id1 = data_[curid - 1][i];
        id2 = data_[curid - 1][i + len / 2];
      }
      if (data[id1] <= data[id2]) {
        data_.back()[i] = id1;
      } else {
        data_.back()[i] = id2;
      }
    }
  }
}

template <typename T>
size_t SparseTable<T>::QueryMin(size_t l, size_t r, const std::vector<T>& data) {
  assert(r > l);
  size_t id = 0;
  size_t tmp = r - l;
  while (tmp > 1) {
    ++id;
    tmp >>= 1;
  }
  if (r - l == 1) {
    return l;
  } else {
    if (r == (1 << id) + l) {
      return data_[id][l];
    } else {
      size_t lid = data_[id][l];
      size_t rid = data_[id][r - (1 << id)];
      assert(lid >= l && lid < r);
      assert(rid >= l && rid < r);
      if (data[lid] <= data[rid]) {
        return lid;
      } else {
        return rid;
      }
    }
  }
}

template <typename T>
std::vector<size_t> SparseTable<T>::Query(
    size_t l, size_t r, const std::vector<T>& data) {
  assert(r > l);
  if (r == l + 1) {
    return {l};
  } else {
    size_t min1_id = QueryMin(l, r, data);
    assert(l <= min1_id && min1_id < r);
    size_t min2_id;
    if (min1_id == l) {
      min2_id = QueryMin(l + 1, r, data);
    } else if (min1_id + 1 == r) {
      min2_id = QueryMin(l, r - 1, data);
    } else {
      size_t q1 = QueryMin(l, min1_id, data);
      size_t q2 = QueryMin(min1_id + 1, r, data);
      if (data[q1] < data[q2]) {
        min2_id = q1;
      } else {
        min2_id = q2;
      }
    }
    return {min1_id, min2_id};
  }
}

#endif  // SPARSE_TABLE_HPP
