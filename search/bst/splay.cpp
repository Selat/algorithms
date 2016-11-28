#include <iostream>
#include <memory>
#include <functional>
#include <set>
#include <cassert>
#include <sstream>
#include <vector>

template <typename T>
class SplayTreeNode {
 public:
  SplayTreeNode(const T& value) : value_(value) {}
  SplayTreeNode(const T& value, std::shared_ptr<SplayTreeNode> parent)
      : value_(value), parent_(parent) {}
  void SetLeft(std::shared_ptr<SplayTreeNode> left) {
    left_ = left;
  }
  void SetRight(std::shared_ptr<SplayTreeNode> right) {
    right_ = right;
  }
  void SetParent(std::shared_ptr<SplayTreeNode> parent) {
    parent_ = parent;
  }
  void SwapValues(SplayTreeNode& other_node) {
    std::swap(value_, other_node.value_);
  }
  void SetValue(const T& value) {
    value_ = value;
  }
  const T& GetValue() const {
    return value_;
  }
  std::shared_ptr<SplayTreeNode> GetLeft() const {
    return left_;
  }
  std::shared_ptr<SplayTreeNode> GetRight() const {
    return right_;
  }
  std::shared_ptr<SplayTreeNode> GetParent() const {
    return parent_.lock();
  }
  std::string Preorder() const {
    std::stringstream res;
    res << value_ << " ";
    res << "(";
    if (left_) {
      assert(this == left_->GetParent().get());
      res << left_->Preorder();
    }
    res << ") (";
    if (right_) {
      assert(this == right_->GetParent().get());
      res << right_->Preorder();
    }
    res << ")";
    return res.str();
  }
 private:
  T value_;
  std::shared_ptr<SplayTreeNode> left_, right_;
  // Parent pointer should be weak in order to prevent cyclic references.
  std::weak_ptr<SplayTreeNode> parent_;
};

template <typename T, typename Comp = std::less<T>>
class SplayTree {
 public:
  using NodePtr = std::shared_ptr<SplayTreeNode<T>> ;
  void Insert(const T& value);
  void Erase(const T& value);
  NodePtr Find(const T& value);
  NodePtr GetLowerBound(const T& value);

  void Clear() {
    root_ = nullptr;
  }

  NodePtr GetRoot() {
    return root_;
  }
 private:
  bool Equiv(const T& value1, const T& value2) const {
    return !comp(value1, value2) && !comp(value2, value1);
  }
  void Splay(NodePtr node);
  void ZigZig(NodePtr& node, NodePtr& parent, NodePtr& grandparent);
  void ZigZag(NodePtr& node, NodePtr& parent, NodePtr& grandparent);
  void ZagZig(NodePtr& node, NodePtr& parent, NodePtr& grandparent);
  void ZagZag(NodePtr& node, NodePtr& parent, NodePtr& grandparent);
  void Zig(NodePtr& node);
  void Zag(NodePtr& node);

  Comp comp;
  NodePtr root_;
};

template <typename T, typename Comp>
void SplayTree<T, Comp>::Insert(const T& value) {
  NodePtr cur_node = root_, parent;
  while (cur_node) {
    parent = cur_node;
    if (comp(value, cur_node->GetValue())) {
      cur_node = cur_node->GetLeft();
    } else if (comp(cur_node->GetValue(), value)) {
      cur_node = cur_node->GetRight();
    } else {
      return;
    }
  }

  cur_node.reset(new SplayTreeNode<T>(value));
  cur_node->SetParent(parent);
  if (!parent) {
    root_ = cur_node;
  } else if (comp(value, parent->GetValue())) {
    parent->SetLeft(cur_node);
  } else {
    parent->SetRight(cur_node);
  }

  Splay(cur_node);
}

template <typename T, typename Comp>
void SplayTree<T, Comp>::Erase(const T& value) {
  auto node = Find(value);
  if (!node) return;
  if (node->GetLeft() && node->GetRight()) {
    auto replacement_node = node->GetRight();
    while (replacement_node->GetLeft()) {
      replacement_node = replacement_node->GetLeft();
    }
    node->SwapValues(*replacement_node);
    node = replacement_node;
  }
  if (node->GetLeft()) {
    node->GetLeft()->SetParent(node->GetParent());
    if (node->GetParent()) {
      if (node->GetParent()->GetLeft() == node) {
        node->GetParent()->SetLeft(node->GetLeft());
      } else {
        node->GetParent()->SetRight(node->GetLeft());
      }
    } else {
      root_ = node->GetLeft();
    }
  } else if (node->GetRight()) {
    node->GetRight()->SetParent(node->GetParent());
    if (node->GetParent()) {
      if (node->GetParent()->GetLeft() == node) {
        node->GetParent()->SetLeft(node->GetRight());
      } else {
        node->GetParent()->SetRight(node->GetRight());
      }
    } else {
      root_ = node->GetRight();
    }
  } else if (node->GetParent()) {
    if (node->GetParent()->GetLeft() == node) {
      node->GetParent()->SetLeft(nullptr);
    } else {
      node->GetParent()->SetRight(nullptr);
    }
  } else {
    root_ = nullptr;
  }
}

template <typename T, typename Comp>
std::shared_ptr<SplayTreeNode<T>> SplayTree<T, Comp>::Find(const T& value) {
  NodePtr cur_node = root_, parent;
  while (cur_node && !Equiv(cur_node->GetValue(), value)) {
    parent = cur_node;
    if (comp(value, cur_node->GetValue())) {
      cur_node = cur_node->GetLeft();
    } else {
      cur_node = cur_node->GetRight();
    }
  }
  if (cur_node && !Equiv(cur_node->GetValue(), value)) {
    return nullptr;
  } else {
    if (cur_node) {
      Splay(cur_node);
    }
    return cur_node;
  }
}

template <typename T, typename Comp>
std::shared_ptr<SplayTreeNode<T>> SplayTree<T, Comp>::GetLowerBound(
    const T& value) {
  NodePtr cur_node = root_;
  NodePtr best_node;
  while (cur_node) {
    if (comp(cur_node->GetValue(), value)) {
      best_node = cur_node;
      cur_node = cur_node->GetRight();
    } else {
      cur_node = cur_node->GetLeft();
    }
  }
  return best_node;
}

template <typename T, typename Comp>
void SplayTree<T, Comp>::Splay(NodePtr node) {
  while (node->GetParent()) {
    auto parent = node->GetParent();
    auto grandparent = parent->GetParent();
    if (grandparent) {
      auto grand_grandparent = grandparent->GetParent();
      bool is_left_son = true;
      if (grand_grandparent && grandparent == grand_grandparent->GetRight()) {
        is_left_son = false;
      }
      if (grandparent->GetLeft() == parent) {
        if (node == node->GetParent()->GetLeft()) {
          ZigZig(node, parent, grandparent);
        } else {
          ZigZag(node, parent, grandparent);
        }
      } else {
        if (node == parent->GetLeft()) {
          ZagZig(node, parent, grandparent);
        } else {
          ZagZag(node, parent, grandparent);
        }
      }
      if (grand_grandparent) {
        if (is_left_son) {
          grand_grandparent->SetLeft(node);
        } else {
          grand_grandparent->SetRight(node);
        }
      }
    } else if (node == parent->GetLeft()) {
      Zig(node);
    } else {
      Zag(node);
    }
  }
}

template <typename T, typename Comp>
void SplayTree<T, Comp>::ZigZig(
    SplayTree::NodePtr& node, SplayTree::NodePtr& parent,
    SplayTree::NodePtr& grandparent) {
  if (parent->GetRight()) {
    parent->GetRight()->SetParent(grandparent);
  }
  grandparent->SetLeft(parent->GetRight());
  parent->SetRight(grandparent);
  if (node->GetRight()) {
    node->GetRight()->SetParent(parent);
  }
  parent->SetLeft(node->GetRight());
  node->SetRight(parent);
  node->SetParent(grandparent->GetParent());
  parent->SetParent(node);
  grandparent->SetParent(parent);
  if (grandparent == root_) {
    root_ = node;
  }
}

template <typename T, typename Comp>
void SplayTree<T, Comp>::ZigZag(
    SplayTree::NodePtr& node, SplayTree::NodePtr& parent,
    SplayTree::NodePtr& grandparent) {
  if (node->GetRight()) {
    node->GetRight()->SetParent(grandparent);
  }
  grandparent->SetLeft(node->GetRight());
  if (node->GetLeft()) {
    node->GetLeft()->SetParent(parent);
  }
  parent->SetRight(node->GetLeft());
  node->SetLeft(parent);
  node->SetRight(grandparent);
  node->SetParent(grandparent->GetParent());
  parent->SetParent(node);
  grandparent->SetParent(node);
  if (grandparent == root_) {
    root_ = node;
  }
}

template <typename T, typename Comp>
void SplayTree<T, Comp>::ZagZig(
    SplayTree::NodePtr& node, SplayTree::NodePtr& parent,
    SplayTree::NodePtr& grandparent) {
  if (node->GetLeft()) {
    node->GetLeft()->SetParent(grandparent);
  }
  grandparent->SetRight(node->GetLeft());
  if (node->GetRight()) {
    node->GetRight()->SetParent(parent);
  }
  parent->SetLeft(node->GetRight());
  node->SetLeft(grandparent);
  node->SetRight(parent);
  node->SetParent(grandparent->GetParent());
  parent->SetParent(node);
  grandparent->SetParent(node);
  if (grandparent == root_) {
    root_ = node;
  }
}

template <typename T, typename Comp>
void SplayTree<T, Comp>::ZagZag(
    SplayTree::NodePtr& node, SplayTree::NodePtr& parent,
    SplayTree::NodePtr& grandparent) {
  if (parent->GetLeft()) {
    parent->GetLeft()->SetParent(grandparent);
  }
  grandparent->SetRight(parent->GetLeft());
  parent->SetLeft(grandparent);
  if (node->GetLeft()) {
    node->GetLeft()->SetParent(parent);
  }
  parent->SetRight(node->GetLeft());
  node->SetLeft(parent);
  node->SetParent(grandparent->GetParent());
  parent->SetParent(node);
  grandparent->SetParent(parent);
  if (grandparent == root_) {
    root_ = node;
  }
}

template <typename T, typename Comp>
void SplayTree<T, Comp>::Zig(SplayTree::NodePtr& node) {
  assert(root_ == node->GetParent());
  if (node->GetRight()) {
    node->GetRight()->SetParent(root_);
  }
  root_->SetLeft(node->GetRight());
  root_->SetParent(node);
  node->SetParent(nullptr);
  node->SetRight(root_);
  root_ = node;
}

template <typename T, typename Comp>
void SplayTree<T, Comp>::Zag(SplayTree::NodePtr& node) {
  assert(root_ == node->GetParent());
  if (node->GetLeft()) {
    node->GetLeft()->SetParent(root_);
  }
  root_->SetRight(node->GetLeft());
  root_->SetParent(node);
  node->SetParent(nullptr);
  node->SetLeft(root_);
  root_ = node;
}

void RunTests() {
  SplayTree<int> splay_tree;
  // Test internal representation
  splay_tree.Insert(3);
  assert("3 () ()" == splay_tree.GetRoot()->Preorder());
  splay_tree.Insert(4);
  assert("4 (3 () ()) ()" == splay_tree.GetRoot()->Preorder());
  splay_tree.Insert(5);
  assert("5 (4 (3 () ()) ()) ()" == splay_tree.GetRoot()->Preorder());
  splay_tree.Clear();
  splay_tree.Insert(5);
  assert("5 () ()" == splay_tree.GetRoot()->Preorder());
  splay_tree.Insert(4);
  assert("4 () (5 () ())" == splay_tree.GetRoot()->Preorder());
  splay_tree.Insert(3);
  assert("3 () (4 () (5 () ()))" == splay_tree.GetRoot()->Preorder());
  splay_tree.Clear();

  splay_tree.Insert(6);
  splay_tree.Insert(7);
  splay_tree.Insert(1);
  splay_tree.Insert(34);
  assert("34 (1 () (7 (6 () ()) ())) ()" == splay_tree.GetRoot()->Preorder());
  splay_tree.Clear();

  std::vector<int> elements;
  std::set<int> s;
  for (int i = 0; i < 100; ++i) {
    elements.push_back(rand() % 10000);
    s.insert(elements.back());
    splay_tree.Insert(elements.back());
  }
  for (auto e_to_remove: elements) {
    for (auto e : s) {
      assert(splay_tree.Find(e));
    }
    s.erase(e_to_remove);
  }
  for (auto e : s) {
    assert(splay_tree.Find(e));
  }

  splay_tree.Clear();
  splay_tree.Insert(1);
  splay_tree.Insert(2);
  splay_tree.Insert(3);
  splay_tree.Insert(4);
  splay_tree.Insert(5);
  auto p = splay_tree.GetLowerBound(6);
  assert(p && p->GetValue() == 5);
  p = splay_tree.GetLowerBound(3);
  assert(p && p->GetValue() == 2);
  p = splay_tree.GetLowerBound(1);
  assert(!p);

  splay_tree.Clear();
  s.clear();
  for (auto e : elements) {
    splay_tree.Insert(e);
    s.insert(e);
  }
  for (int i = 0; i < 1000; ++i) {
    int rand_id = rand() % elements.size();
    splay_tree.Erase(elements[rand_id]);
    splay_tree.Insert(elements[rand_id]);
    for (auto e : s) {
      assert(splay_tree.Find(e));
    }
  }

  splay_tree.Clear();
  splay_tree.Insert(5);
  splay_tree.Find(100);

  std::cout << "All tests passed!" << std::endl;
}

class RoomsRange {
 public:
  RoomsRange() : start_(0), length_(0) {}
  RoomsRange(int start, int length, int id)
      : start_(start), length_(length), id_(id)  {}
  void SetStart(int start) {
    start_ = start;
  }
  void SetLength(int length) {
    length_ = length;
  }
  int GetStart() const {
    return start_;
  }
  int GetLength() const {
    return length_;
  }
  int GetId() const {
    return id_;
  }
  int GetRightmost() const {
    return start_ + length_ - 1;
  }
  bool operator<(const RoomsRange& range) const {
    return start_ < range.start_;
  }
 private:
  int start_;
  int length_;
  int id_;
};

int main() {
  // RunTests();
  SplayTree<RoomsRange> splay_tree;
  int queries_num;
  std::cin >> queries_num;
  std::vector<RoomsRange> checkins;
  while (queries_num--) {
    int query_type;
    std::cin >> query_type;
    if (query_type == 1) {
      int start, length;
      std::cin >> start >> length;
      checkins.push_back(RoomsRange(start, length, checkins.size()));
      RoomsRange cur_checkin(start + length, length, 0);
      auto lower_bound_pointer = splay_tree.GetLowerBound(cur_checkin);
      if (lower_bound_pointer &&
          lower_bound_pointer->GetValue().GetRightmost() >= start) {
        std::cout << "0";
      } else {
        std::cout << "1";
        splay_tree.Insert(checkins.back());
      }
    } else {
      int checkin_id;
      std::cin >> checkin_id;
      auto checkin_pointer = splay_tree.Find(checkins[checkin_id]);
      if (checkin_pointer &&
          checkin_pointer->GetValue().GetId() == checkin_id) {
        splay_tree.Erase(checkins[checkin_id]);
        std::cout << "1";
      } else {
        std::cout << "0";
      }
    }
  }
  std::cout << std::endl;
  return 0;
}
