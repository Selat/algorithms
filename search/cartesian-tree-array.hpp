#ifndef CARTESIAN_TREE_ARRAY_HPP
#define CARTESIAN_TREE_ARRAY_HPP

#include <cassert>
#include <vector>
#include <cstdlib>

template <typename T>
class CartesianTreeArray {
 public:
  void Init(const std::vector<T>& data) {
    assert(!data.empty());
    int cur_node = 0;
    data_ = data;
    root_id_ = 0;
    left_.assign(data.size(), -1);
    right_.assign(data.size(), -1);
    parent_.assign(data.size(), -1);
    // We have to always store owning pointer to the tree root in order to
    // prevent it from destructing (cur_node stores only weak pointer to it's
    // parent).
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
    // for (auto p : parent_) {
    //   std::cout << p << " ";
    // }
    // std::cout << std::endl;
  }
  void CheckHeapProperty() {
    if (root_id_ >= 0) {
      CheckHeapProperty(root_id_, data_[root_id_]);
    }
  }
  void CheckHeapProperty(int id, int min_value) {
    assert(data_[id] >= min_value);
    if (left_[id] != -1) {
      CheckHeapProperty(left_[id], data_[id]);
    }
    if (right_[id] != -1) {
      CheckHeapProperty(right_[id], data_[id]);
    }
  }
  void Dfs(std::vector<T>& a) {
    Dfs(a, root_id_);
  }
  void Dfs(std::vector<T>& a, int id) {
    if (id == -1) return;
    Dfs(a, left_[id]);
    a.push_back(data_[id]);
    Dfs(a, right_[id]);
  }
 private:
  int root_id_;
  std::vector<T> data_;
  std::vector<int> left_;
  std::vector<int> right_;
  std::vector<int> parent_;
};

#endif  // CARTESIAN_TREE_ARRAY_HPP
