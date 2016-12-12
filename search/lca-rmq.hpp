#ifndef LCA_RMQ_HPP
#define LCA_RMQ_HPP

#include <vector>
#include <iostream>
#include <cmath>

#include "cartesian-tree-array.hpp"
#include "sparse-table.hpp"

// Idea: http://www3.cs.stonybrook.edu/~bender/pub/lca.ps
template <typename T>
class RmqLca {
 public:
  void Init(const std::vector<T>& data);
  std::vector<size_t> Query(size_t l, size_t r, const std::vector<T>& data);
  size_t GetMinId(uint32_t block_type, size_t l, size_t r);
  size_t QueryMin(size_t l, size_t r);
  RmqLca() {}
private:
  size_t block_size_;
  std::vector<int> blocks_mins_;
  std::vector<uint32_t> block_types_;
  SparseTable<T> sparse_table_;
  std::vector<uint32_t> precomputed_blocks_mins_;
  std::vector<int> representatives_;
  std::vector<int> levels_;
  std::vector<int> original_ids_;
};


template <typename T>
void RmqLca<T>::Init(const std::vector<T>& data) {
  CartesianTreeArray<T> cartesian_tree;
  cartesian_tree.Init(data);
  cartesian_tree.Dfs(representatives_, levels_, original_ids_);
  int block_size = ceil(log2(original_ids_.size()) / 2.0);
  assert(block_size > 1);
  uint32_t total_size = 1 << (block_size - 1);
  precomputed_blocks_mins_.resize(total_size);
  std::vector<int> blocks_mins;
  for (size_t i = 0; i < levels_.size(); i += block_size) {
    uint32_t block_type = 0;
    size_t last = std::min(levels_.size(), i + block_size);
    size_t min_id = i;
    for (size_t j = i + 1; j < last; ++j) {
      if (levels_[j] < levels_[min_id]) {
        min_id = j;
      }
      block_type = (block_type << 1);
      if (levels_[j] > levels_[j - 1]) {
        block_type |= 1;
      }
    }
    blocks_mins.push_back(levels_[min_id]);
    if (last < i + block_size) {
      block_type <<= (block_size + i - last);
      // Make sure that missing elements won't became minimums;
      block_type |= (1 << (block_size + i - last)) - 1;
    }
    assert(block_type < total_size);
    block_types_.push_back(block_type);
  }
  for (size_t i = 0; i < precomputed_blocks_mins_.size(); ++i) {
    int cur_value = 0;
    unsigned char min_id = 0;
    int min_value = 0;
    for (size_t j = 1; j < block_size; ++j) {
      if (i & (1 << (block_size - j - 1))) {
        ++cur_value;
      } else {
        --cur_value;
      }
      if (cur_value < min_value) {
        min_value = cur_value;
        min_id = j;
      }
    }
    precomputed_blocks_mins_[i] = min_id;
  }
  blocks_mins_ = blocks_mins;
  blocks_mins.clear();
  sparse_table_.Init(blocks_mins_);
  block_size_ = block_size;
}

template <typename T>
std::vector<size_t> RmqLca<T>::Query(
    size_t l, size_t r, const std::vector<T>& data) {
  assert(r > l);
  if (r == l + 1) {
    return {l};
  } else {
    size_t min1_id = QueryMin(l, r);
    assert(l <= min1_id && min1_id < r);
    size_t min2_id;
    if (min1_id == l) {
      min2_id = QueryMin(l + 1, r);
    } else if (min1_id + 1 == r) {
      min2_id = QueryMin(l, r - 1);
    } else {
      size_t q1 = QueryMin(l, min1_id);
      size_t q2 = QueryMin(min1_id + 1, r);
      if (data[q1] < data[q2]) {
        min2_id = q1;
      } else {
        min2_id = q2;
      }
    }
    return {min1_id, min2_id};
  }
}

template <typename T>
size_t RmqLca<T>::GetMinId(uint32_t block_type, size_t l, size_t r) {
  block_type |= (1 << (block_size_ - r - 1)) - 1;
  block_type &= (1 << (block_size_ - l - 1)) - 1;
  assert(block_type < precomputed_blocks_mins_.size());
  assert(precomputed_blocks_mins_[block_type] >= l &&
         precomputed_blocks_mins_[block_type] <= r);
  return precomputed_blocks_mins_[block_type];
}

template <typename T>
size_t RmqLca<T>::QueryMin(size_t l, size_t r) {
  assert(r > l);
  l = representatives_[l];
  r = representatives_[r - 1];
  if (r < l) {
    std::swap(l, r);
  }
  size_t blid = l / block_size_;
  if (l % block_size_) {
    ++blid;
  }
  size_t brid = r / block_size_;
  if ((r % block_size_) == block_size_ - 1) {
    ++brid;
  }
  size_t res_id = -1;
  if (brid > blid) {
    size_t bid = sparse_table_.QueryMin(blid, brid, blocks_mins_);
    uint32_t block_type = block_types_[bid];
    size_t min_id = bid * block_size_ + GetMinId(block_type, 0, block_size_ - 1);
    res_id = min_id;
  }
  // [l; r) belongs to one block
  if ((r - l) < block_size_ && (r % block_size_) >= (l % block_size_)) {
    size_t tlid = l % block_size_;
    size_t trid = r % block_size_;
    uint32_t block_type = block_types_[l / block_size_];
    size_t min_id = l - (l % block_size_) + GetMinId(block_type, tlid, trid);
    if (res_id == -1 || levels_[min_id] < levels_[res_id]) {
      res_id = min_id;
    }
  } else {
    if (l % block_size_) {
      size_t tlid = l % block_size_;
      size_t trid = block_size_ - 1;
      uint32_t block_type = block_types_[l / block_size_];
      size_t min_id = l - (l % block_size_) +
                      GetMinId(block_type, tlid, trid);
      if (res_id == -1 || levels_[min_id] < levels_[res_id]) {
        res_id = min_id;
      }
    }
    if (r % block_size_ < block_size_ - 1) {
      size_t tlid = 0;
      size_t trid = r % block_size_;
      uint32_t block_type = block_types_[r / block_size_];
      size_t min_id = r - (r % block_size_) +
                      GetMinId(block_type, tlid, trid);
      if (res_id == -1 || levels_[min_id] < levels_[res_id]) {
        res_id = min_id;
      }
    }
  }
  assert(res_id != -1);
  return original_ids_[res_id];
}

#endif  // LCA_RMQ_HPP
