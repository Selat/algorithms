#ifndef LCA_RMQ_HPP
#define LCA_RMQ_HPP

#include <vector>
#include <iostream>
#include <cmath>

#include "cartesian-tree.hpp"
#include "sparse-table.hpp"

template <typename T>
void Dfs(typename CartesianTree<T>::NodePtr node, int& id, int cur_level, std::vector<int>& representatives, std::vector<int>& levels, std::vector<int>& original_ids) {
  if (representatives[node->GetId()] == -1) {
    representatives[node->GetId()] = id;
  }
  original_ids[id] = node->GetId();
  levels[id++] = cur_level;
  if (node->GetLeft()) {
    Dfs<T>(node->GetLeft(), id, cur_level + 1, representatives, levels, original_ids);
    original_ids[id] = node->GetId();
    levels[id++] = cur_level;
  }
  if (node->GetRight()) {
    Dfs<T>(node->GetRight(), id, cur_level + 1, representatives, levels, original_ids);
    original_ids[id] = node->GetId();
    levels[id++] = cur_level;
  }
}

template <typename T>
void EulerRoute(const CartesianTree<T>& tree, std::vector<int>& representatives, std::vector<int>& levels, std::vector<int>& original_ids) {
  representatives.assign(tree.GetNodesNum(), -1);
  levels.resize(2 * tree.GetNodesNum() - 1);
  original_ids.resize(2 * tree.GetNodesNum() - 1);
  assert(tree.GetRoot());
  int id = 0;
  Dfs<T>(tree.GetRoot(), id, 0, representatives, levels, original_ids);
}

template <typename T>
class RmqLca {
public:
  static RmqLca Init(const std::vector<T>& data) {
    RmqLca res;
    auto cartesian_tree = CartesianTree<T>::Init(data);
    res.source_data_ = data;
    EulerRoute(cartesian_tree, res.representatives_, res.levels_, res.original_ids_);
    int block_size = ceil(log2(res.original_ids_.size()) / 2.0);
    assert(block_size > 1);
    res.precomputed_blocks_mins_.resize(1 << (block_size - 1));
    std::vector<bool> is_initialized(res.precomputed_blocks_mins_.size(), false);
    std::vector<T> blocks_mins;
    for (size_t i = 0; i < res.levels_.size(); i += block_size) {
      uint32_t block_type = 0;
      size_t last = std::min(res.levels_.size(), i + block_size);
      size_t min_id = i;
      for (size_t j = i + 1; j < last; ++j) {
        if (res.levels_[j] < res.levels_[min_id]) {
          min_id = j;
        }
        block_type = (block_type << 1);
        if (res.levels_[j] > res.levels_[j - 1]) {
          block_type |= 1;
        }
      }
      blocks_mins.push_back(res.levels_[min_id]);
      if (last < i + block_size) {
        block_type <<= (block_size + i - last);
        // Make sure that missing elements won't became minimums;
        block_type |= (1 << (block_size + i - last)) - 1;
      }
      assert(block_type < res.precomputed_blocks_mins_.size());
      if (!is_initialized[block_type]) {
        res.precomputed_blocks_mins_[block_type].assign(block_size, std::vector<size_t>(block_size, std::numeric_limits<size_t>::max()));
        for (size_t k = i; k < last; ++k) {
          size_t min_id = k;
          res.precomputed_blocks_mins_[block_type][k - i][k - i] = min_id - i;
          for (size_t l = k + 1; l < last; ++l) {
            if (res.levels_[l] < res.levels_[min_id]) {
              min_id = l;
            }
            res.precomputed_blocks_mins_[block_type][k - i][l - i] = min_id - i;
          }
        }
        is_initialized[block_type] = true;
      }
      res.block_types_.push_back(block_type);
    }
    res.sparse_table_ = SparseTable<T>::Init(blocks_mins);
    res.block_size_ = block_size;
    return res;
  }
  size_t Query(size_t l, size_t r) {
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
    std::vector<size_t> min_ids;
    if (brid > blid) {
      size_t bid = sparse_table_.Query(blid, brid);
      uint32_t block_type = block_types_[bid];
      size_t min_id = bid * block_size_ + precomputed_blocks_mins_[block_type][0][block_size_ - 1];
      min_ids.push_back(original_ids_[min_id]);
    }
    // [l; r) belongs to one block
    if ((r - l) < block_size_ && (r % block_size_) >= (l % block_size_)) {
      size_t tlid = l % block_size_;
      size_t trid = r % block_size_;
      uint32_t block_type = block_types_[l / block_size_];
      assert(precomputed_blocks_mins_[block_type][tlid][trid] != std::numeric_limits<size_t>::max());
      size_t min_id = (l - (l % block_size_)) + precomputed_blocks_mins_[block_type][tlid][trid];
      min_ids.push_back(original_ids_[min_id]);
    } else {
      if (l % block_size_) {
        size_t tlid = l % block_size_;
        size_t trid = block_size_ - 1;
        uint32_t block_type = block_types_[l / block_size_];
        assert(precomputed_blocks_mins_[block_type][tlid][trid] != std::numeric_limits<size_t>::max());
        size_t min_id = (l - (l % block_size_)) + precomputed_blocks_mins_[block_type][tlid][trid];
        min_ids.push_back(original_ids_[min_id]);
      }
      if (r % block_size_ < block_size_ - 1) {
        size_t tlid = 0;
        size_t trid = r % block_size_;
        uint32_t block_type = block_types_[r / block_size_];
        assert(precomputed_blocks_mins_[block_type][tlid][trid] != std::numeric_limits<size_t>::max());
        size_t min_id = (r - (r % block_size_)) + precomputed_blocks_mins_[block_type][tlid][trid];
        min_ids.push_back(original_ids_[min_id]);
      }
    }
    size_t min_id = min_ids.back();
    for (auto id : min_ids) {
      if (source_data_[id] < source_data_[min_id]) {
        min_id = id;
      }
    }
    return min_id;
  }
private:
  RmqLca() {}
  std::vector<T> source_data_;
  size_t block_size_;
  std::vector<uint32_t> block_types_;
  SparseTable<T> sparse_table_;
  std::vector<std::vector<std::vector<size_t>>> precomputed_blocks_mins_;
  std::vector<int> representatives_;
  std::vector<int> levels_;
  std::vector<int> original_ids_;
};

#endif  // LCA_RMQ_HPP
