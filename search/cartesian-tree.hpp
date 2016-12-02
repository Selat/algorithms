#ifndef CARTESIAN_TREE_HPP
#define CARTESIAN_TREE_HPP

#include <cassert>
#include <vector>
#include <memory>
#include <sstream>

template <typename T>
class CartesianTreeNode {
 public:
  CartesianTreeNode(const T& value, int id) : value_(value), id_(id) {}
  CartesianTreeNode(const T& value, int id,
                    std::shared_ptr<CartesianTreeNode> parent)
      : value_(value), id_(id), parent_(parent) {}
  void SetLeft(std::shared_ptr<CartesianTreeNode> left) {
    left_ = left;
  }
  void SetRight(std::shared_ptr<CartesianTreeNode> right) {
    right_ = right;
  }
  void SetParent(std::shared_ptr<CartesianTreeNode> parent) {
    parent_ = parent;
  }
  void SwapValues(CartesianTreeNode& other_node) {
    std::swap(value_, other_node.value_);
  }
  void SetValue(const T& value) {
    value_ = value;
  }
  const T& GetValue() const {
    return value_;
  }
  void SetId(int id) {
    id_ = id;
  }
  int GetId() const {
    return id_;
  }
  std::shared_ptr<CartesianTreeNode> GetLeft() const {
    return left_;
  }
  std::shared_ptr<CartesianTreeNode> GetRight() const {
    return right_;
  }
  std::shared_ptr<CartesianTreeNode> GetParent() const {
    return parent_.lock();
  }
  void CheckHeapProperty(int min_val) const {
    assert(value_ >= min_val);
    if (left_) {
      assert(this == left_->GetParent().get());
      left_->CheckHeapProperty(value_);
    }
    if (right_) {
      assert(this == right_->GetParent().get());
      right_->CheckHeapProperty(value_);
    }
  }
  void Inorder(std::vector<T>& data) const {
    if (left_) {
      left_->Inorder(data);
    }
    data.push_back(value_);
    if (right_) {
      right_->Inorder(data);
    }
  }
 private:
  int id_;
  T value_;
  std::shared_ptr<CartesianTreeNode> left_, right_;
  // Parent pointer should be weak in order to prevent cyclic references.
  std::weak_ptr<CartesianTreeNode> parent_;
};

template <typename T>
class CartesianTree {
 public:
  using NodePtr = std::shared_ptr<CartesianTreeNode<T>>;
  static CartesianTree Init(const std::vector<T>& data) {
    assert(!data.empty());
    CartesianTree res;
    res.nodes_num_ = data.size();
    NodePtr cur_node = NodePtr(new CartesianTreeNode<T>(data[0], 0));
    res.root_ = cur_node;
    // We have to always store owning pointer to the tree root in order to
    // prevent it from destructing (cur_node stores only weak pointer to it's
    // parent).
    for (size_t i = 1; i < data.size(); ++i) {
      while (cur_node->GetParent() && cur_node->GetValue() > data[i]) {
        cur_node = cur_node->GetParent();
      }
      NodePtr new_node(new CartesianTreeNode<T>(data[i], i));
      if (cur_node->GetValue() <= data[i]) {
        if (cur_node->GetRight()) {
          new_node->SetLeft(cur_node->GetRight());
          cur_node->GetRight()->SetParent(new_node);
        }
        cur_node->SetRight(new_node);
        new_node->SetParent(cur_node);
      } else {
        assert(!cur_node->GetParent());
        new_node->SetLeft(cur_node);
        cur_node->SetParent(new_node);
        if (cur_node == res.root_) {
          res.root_ = new_node;
        }
      }
      cur_node = new_node;
    }
    return res;
  }
  NodePtr GetRoot() const {
    return root_;
  }
  void CheckHeapProperty() {
    if (root_) {
      root_->CheckHeapProperty(root_->GetValue());
    }
  }
  int GetNodesNum() const {
    return nodes_num_;
  }
 private:
  CartesianTree() {}
  NodePtr root_;
  int nodes_num_;
};

#endif  // CARTESIAN_TREE_HPP
