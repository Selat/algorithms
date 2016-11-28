#include <iostream>
#include <memory>
#include <functional>
#include <set>
#include <cassert>
#include <sstream>
#include <vector>

template <typename T>
class TreeNode {
 public:
  enum Color {RED, BLACK};
  TreeNode(const T& value) : value_(value) {}
  TreeNode(const T& value, std::shared_ptr<TreeNode> parent)
      : value_(value), parent_(parent) {}
  void SetLeft(std::shared_ptr<TreeNode> left) {
    left_ = left;
  }
  void SetRight(std::shared_ptr<TreeNode> right) {
    right_ = right;
  }
  void SetParent(std::shared_ptr<TreeNode> parent) {
    parent_ = parent;
  }
  void SetColor(Color color) {
    color_ = color;
  }
  void SwapValues(TreeNode& other_node) {
    std::swap(value_, other_node.value_);
  }
  void SetValue(const T& value) {
    value_ = value;
  }
  const T& GetValue() const {
    return value_;
  }
  std::shared_ptr<TreeNode> GetLeft() const {
    return left_;
  }
  std::shared_ptr<TreeNode> GetRight() const {
    return right_;
  }
  std::shared_ptr<TreeNode> GetParent() const {
    return parent_.lock();
  }
  Color GetColor() const {
    return color_;
  }
  int CheckRBTreeProperty() const {
    if (color_ == BLACK) {
      int l = 1;
      if (left_) {
        l = left_->CheckRBTreeProperty();
      }
      int r = 1;
      if (right_) {
        r = right_->CheckRBTreeProperty();
      }
      assert(l == r);
      return l + 1;
    } else {
      int l = 1;
      if (left_) {
        assert(left_->color_ == BLACK);
        l = left_->CheckRBTreeProperty();
      }
      int r = 1;
      if (right_) {
        assert(right_->color_ == BLACK);
        r = right_->CheckRBTreeProperty();
      }
      assert(l == r);
      return l;
    }
  }
  std::string Preorder() const {
    std::stringstream res;
    res << value_ << ((color_ == RED)?'@':' ') << " ";
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
  Color color_;
  T value_;
  std::shared_ptr<TreeNode> left_, right_;
  // Parent pointer should be weak in order to prevent cyclic references.
  std::weak_ptr<TreeNode> parent_;
};

template <typename T, typename Comp = std::less<T>>
class RedBlackTree {
 public:
  using NodePtr = std::shared_ptr<TreeNode<T>> ;
  void Insert(const T& value);
  void Erase(const T& value);
  NodePtr Find(const T& value) const;
  NodePtr GetLowerBound(const T& value);

  void CheckRBTreeProperty() const {
    if (root_) {
      assert(root_->GetColor() == TreeNode<T>::BLACK);
      root_->CheckRBTreeProperty();
    }
  }

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
  static NodePtr GetUncle(NodePtr node) {
    auto parent = node->GetParent();
    if (!parent) return nullptr;
    auto grandparent = parent->GetParent();
    if (!grandparent) return nullptr;
    if (parent == grandparent->GetRight()) {
      return grandparent->GetLeft();
    } else {
      return grandparent->GetRight();
    }
  }
  static NodePtr GetGrandparent(NodePtr node) {
    if (node->GetParent()) {
      return node->GetParent()->GetParent();
    } else {
      return nullptr;
    }
  }

  void LeftRotate(NodePtr nodex) {
    auto nodey = nodex->GetRight();
    assert(nodey);
    auto parent = nodex->GetParent();
    nodey->SetParent(parent);
    if (parent) {
      if (nodex == parent->GetLeft()) {
        parent->SetLeft(nodey);
      } else {
        parent->SetRight(nodey);
      }
    } else {
      root_ = nodey;
    }

    if (nodey->GetLeft()) {
      nodey->GetLeft()->SetParent(nodex);
    }
    nodex->SetRight(nodey->GetLeft());
    nodey->SetLeft(nodex);
    nodex->SetParent(nodey);
  }

  void RightRotate(NodePtr nodey) {
    auto nodex = nodey->GetLeft();
    assert(nodex);
    auto parent = nodey->GetParent();
    nodex->SetParent(parent);
    if (parent) {
      if (nodey == parent->GetLeft()) {
        parent->SetLeft(nodex);
      } else {
        parent->SetRight(nodex);
      }
    } else {
      root_ = nodex;
    }

    if (nodex->GetRight()) {
      nodex->GetRight()->SetParent(nodey);
    }
    nodey->SetLeft(nodex->GetRight());
    nodex->SetRight(nodey);
    nodey->SetParent(nodex);
  }

  // This implementation is based on algorithm description provided at
  // https://en.wikipedia.org/wiki/Red%E2%80%93black_tree
  // node is root
  void InsertCase1(NodePtr node);
  // node parent is black
  void InsertCase2(NodePtr node);
  // node uncle is red
  void InsertCase3(NodePtr node);
  void InsertCase4(NodePtr node);
  void InsertCase5(NodePtr node);

  Comp comp;
  NodePtr root_;
};

template <typename T, typename Comp>
void RedBlackTree<T, Comp>::Insert(const T& value) {
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

  cur_node.reset(new TreeNode<T>(value));
  cur_node->SetColor(TreeNode<T>::RED);
  cur_node->SetParent(parent);
  if (!parent) {
    root_ = cur_node;
  } else if (comp(value, parent->GetValue())) {
    parent->SetLeft(cur_node);
  } else {
    parent->SetRight(cur_node);
  }

  InsertCase1(cur_node);
}

template <typename T, typename Comp>
void RedBlackTree<T, Comp>::Erase(const T& value) {
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
std::shared_ptr<TreeNode<T>> RedBlackTree<T, Comp>::Find(const T& value) const {
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
    return cur_node;
  }
}

template <typename T, typename Comp>
std::shared_ptr<TreeNode<T>> RedBlackTree<T, Comp>::GetLowerBound(
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
void RedBlackTree<T, Comp>::InsertCase1(NodePtr node) {
  if (!node->GetParent()) {
    node->SetColor(TreeNode<T>::BLACK);
  } else {
    InsertCase2(node);
  }
}

template <typename T, typename Comp>
void RedBlackTree<T, Comp>::InsertCase2(NodePtr node) {
  if (node->GetParent()->GetColor() != TreeNode<T>::BLACK) {
    InsertCase3(node);
  }
}

template <typename T, typename Comp>
void RedBlackTree<T, Comp>::InsertCase3(NodePtr node) {
  auto uncle = GetUncle(node);
  if (uncle && uncle->GetColor() == TreeNode<T>::RED) {
    node->GetParent()->SetColor(TreeNode<T>::BLACK);
    uncle->SetColor(TreeNode<T>::BLACK);
    auto grandparent = GetGrandparent(node);
    grandparent->SetColor(TreeNode<T>::RED);
    InsertCase1(grandparent);
  } else {
    InsertCase4(node);
  }
}

template <typename T, typename Comp>
void RedBlackTree<T, Comp>::InsertCase4(NodePtr node) {
  auto parent = node->GetParent();
  // parent is red => grandparent always exists
  auto grandparent = parent->GetParent();
  if (node == parent->GetRight() && parent == grandparent->GetLeft()) {
    LeftRotate(parent);
    node = node->GetLeft();
  } else if (node == parent->GetLeft() && parent == grandparent->GetRight()) {
    RightRotate(parent);
    node = node->GetRight();
  }
  InsertCase5(node);
}

template <typename T, typename Comp>
void RedBlackTree<T, Comp>::InsertCase5(NodePtr node) {
  auto parent = node->GetParent();
  // parent is red => grandparent always exists
  auto grandparent = parent->GetParent();
  auto grand_grandparent = grandparent->GetParent();
  parent->SetColor(TreeNode<T>::BLACK);
  grandparent->SetColor(TreeNode<T>::RED);
  if (node == parent->GetLeft()) {
    RightRotate(grandparent);
  } else {
    LeftRotate(grandparent);
  }
}


void RunTests() {
  RedBlackTree<int> rb_tree;
  // Test internal representation
  // rb_tree.Insert(3);
  // assert("3 () ()" == rb_tree.GetRoot()->Preorder());
  // rb_tree.Insert(4);
  // assert("4 (3 () ()) ()" == rb_tree.GetRoot()->Preorder());
  // rb_tree.Insert(5);
  // assert("5 (4 (3 () ()) ()) ()" == rb_tree.GetRoot()->Preorder());
  // rb_tree.Clear();
  // rb_tree.Insert(5);
  // assert("5 () ()" == rb_tree.GetRoot()->Preorder());
  // rb_tree.Insert(4);
  // assert("4 () (5 () ())" == rb_tree.GetRoot()->Preorder());
  // rb_tree.Insert(3);
  // assert("3 () (4 () (5 () ()))" == rb_tree.GetRoot()->Preorder());
  // rb_tree.Clear();
  //
  // rb_tree.Insert(6);
  // rb_tree.Insert(7);
  // rb_tree.Insert(1);
  // rb_tree.Insert(34);
  // assert("34 (1 () (7 (6 () ()) ())) ()" == rb_tree.GetRoot()->Preorder());
  // rb_tree.Clear();

  std::vector<int> elements;
  std::set<int> s;
  for (int i = 0; i < 10000; ++i) {
    elements.push_back(rand());
    s.insert(elements.back());
    rb_tree.Insert(elements.back());
    rb_tree.CheckRBTreeProperty();
  }
  // for (auto e_to_remove: elements) {
  //   for (auto e : s) {
  //     assert(rb_tree.Find(e));
  //   }
  //   s.erase(e_to_remove);
  // }
  // for (auto e : s) {
  //   assert(rb_tree.Find(e));
  // }

  rb_tree.Clear();
  rb_tree.Insert(1);
  rb_tree.Insert(2);
  rb_tree.Insert(3);
  rb_tree.Insert(4);
  rb_tree.Insert(5);
  auto p = rb_tree.GetLowerBound(6);
  assert(p && p->GetValue() == 5);
  p = rb_tree.GetLowerBound(3);
  assert(p && p->GetValue() == 2);
  p = rb_tree.GetLowerBound(1);
  assert(!p);

  rb_tree.Clear();
  s.clear();
  // for (auto e : elements) {
  //   rb_tree.Insert(e);
  //   s.insert(e);
  // }
  // for (int i = 0; i < 1000; ++i) {
  //   int rand_id = rand() % elements.size();
  //   rb_tree.Erase(elements[rand_id]);
  //   rb_tree.Insert(elements[rand_id]);
  //   for (auto e : s) {
  //     assert(rb_tree.Find(e));
  //   }
  // }
  //
  // rb_tree.Clear();
  // rb_tree.Insert(5);
  // rb_tree.Find(100);

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
  RunTests();
  RedBlackTree<RoomsRange> rb_tree;
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
      auto lower_bound_pointer = rb_tree.GetLowerBound(cur_checkin);
      if (lower_bound_pointer &&
          lower_bound_pointer->GetValue().GetRightmost() >= start) {
        std::cout << "0";
      } else {
        std::cout << "1";
        rb_tree.Insert(checkins.back());
      }
    } else {
      int checkin_id;
      std::cin >> checkin_id;
      auto checkin_pointer = rb_tree.Find(checkins[checkin_id]);
      if (checkin_pointer &&
          checkin_pointer->GetValue().GetId() == checkin_id) {
        rb_tree.Erase(checkins[checkin_id]);
        std::cout << "1";
      } else {
        std::cout << "0";
      }
    }
  }
  std::cout << std::endl;
  return 0;
}
