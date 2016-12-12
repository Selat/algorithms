#ifndef CARTESIAN_TREE_ARRAY_HPP
#define CARTESIAN_TREE_ARRAY_HPP

#include <cassert>
#include <vector>
#include <cstdlib>

template <typename T>
class CartesianTreeArray {
 public:
  void Init(const std::vector<T>& data);
  // Uses O(1) extra memory and destructs the tree.
  void Dfs(std::vector<int>& representatives, std::vector<int>& levels,
           std::vector<int>& original_ids);
 private:
  int root_id_;
  std::vector<T> data_;
  std::vector<int> left_;
  std::vector<int> right_;
  std::vector<int> parent_;
};

template <typename T>
void CartesianTreeArray<T>::Init(const std::vector<T>& data) {
  assert(!data.empty());
  int cur_node = 0;
  data_ = data;
  root_id_ = 0;
  left_.assign(data.size(), -1);
  right_.assign(data.size(), -1);
  parent_.assign(data.size(), -1);
  for (size_t i = 1; i < data.size(); ++i) {
    while (parent_[cur_node] != -1 && data[cur_node] > data[i]) {
      cur_node = parent_[cur_node];
    }
    if (data[cur_node] <= data[i]) {
      if (right_[cur_node] >= 0) {
        left_[i] = right_[cur_node];
        parent_[right_[cur_node]] = i;
      }
      right_[cur_node] = i;
      parent_[i] = cur_node;
    } else {
      assert(parent_[cur_node] == -1);
      left_[i] = cur_node;
      parent_[cur_node] = i;
      if (cur_node == root_id_) {
        root_id_ = i;
      }
    }
    cur_node = i;
  }
}

template <typename T>
void CartesianTreeArray<T>::Dfs(
    std::vector<int>& representatives, std::vector<int>& levels,
    std::vector<int>& original_ids) {
  representatives.assign(parent_.size(), -1);
  levels.resize(2 * parent_.size() - 1);
  original_ids.resize(2 * parent_.size() - 1);
  bool should_update = true;
  int node = root_id_;
  int id = 0;
  int cur_level = 0;
  while (node != -1) {
    if (representatives[node] == -1) {
      representatives[node] = id;
    }
    if (should_update) {
      original_ids[id] = node;
      levels[id++] = cur_level;
    }
    if (left_[node] != -1) {
      node = left_[node];
      should_update = true;
      ++cur_level;
    } else if (right_[node] != -1) {
      should_update = true;
      node = right_[node];
      ++cur_level;
    } else {
      node = parent_[node];
      should_update = false;
      --cur_level;
      if (node != -1) {
        if (left_[node] != -1) {
          original_ids[id] = node;
          levels[id++] = cur_level;
          left_[node] = -1;
        } else if (right_[node] != -1) {
          right_[node] = -1;
          original_ids[id] = node;
          levels[id++] = cur_level;
        }
      }
    }
  }
}

#endif  // CARTESIAN_TREE_ARRAY_HPP
