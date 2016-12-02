#ifndef LCA_RMQ_HPP
#define LCA_RMQ_HPP

#include <vector>
#include <iostream>

#include "cartesian-tree.hpp"

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
    for (auto e : data) {
      std::cout << e << " ";
    }
    std::cout << std::endl;
    EulerRoute(cartesian_tree, res.representatives_, res.levels_, res.original_ids_);
    for (int i = 0; i < res.representatives_.size(); ++i) {
      std::cout << res.representatives_[i] << " ";
    }
    std::cout << std::endl;
    for (int i = 0; i < res.levels_.size(); ++i) {
      std::cout << res.levels_[i] << " ";
    }
    std::cout << std::endl;
    for (int i = 0; i < res.original_ids_.size(); ++i) {
      std::cout << res.original_ids_[i] << " ";
    }
    std::cout << std::endl;
    return res;
  }
private:
  RmqLca() {}
  std::vector<int> representatives_;
  std::vector<int> levels_;
  std::vector<int> original_ids_;
};

#endif  // LCA_RMQ_HPP
