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
  std::string Preorder() {
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
  std::weak_ptr<SplayTreeNode> parent_;
};

template <typename T, typename Comp = std::less<T>>
class SplayTree {
 public:
  void Insert(const T& value) {
    std::shared_ptr<SplayTreeNode<T>> cur = root_, parent;
    while (cur) {
      parent = cur;
      if (comp(value, cur->GetValue())) {
        cur = cur->GetLeft();
      } else if (comp(cur->GetValue(), value)) {
        cur = cur->GetRight();
      } else {
        return;
      }
    }

    cur.reset(new SplayTreeNode<T>(value));
    cur->SetParent(parent);
    if (!parent) {
      root_ = cur;
    } else if (comp(value, parent->GetValue())) {
      parent->SetLeft(cur);
    } else {
      parent->SetRight(cur);
    }

    Splay(cur);
  }

  void Erase(const T& value) {
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

  std::shared_ptr<SplayTreeNode<T>> Find(const T& value) {
    std::shared_ptr<SplayTreeNode<T>> cur = root_, parent;
    while (cur && (comp(cur->GetValue(), value) || comp(value, cur->GetValue()))) {
      parent = cur;
      if (comp(value, cur->GetValue())) {
        cur = cur->GetLeft();
      } else {
        cur = cur->GetRight();
      }
    }
    if (cur && (comp(cur->GetValue(), value) || comp(value, cur->GetValue()))) {
      return nullptr;
    } else {
      return cur;
    }
  }

  std::shared_ptr<SplayTreeNode<T>> GetLowerBound(const T& value) {
    std::shared_ptr<SplayTreeNode<T>> cur = root_;
    std::shared_ptr<SplayTreeNode<T>> best_node;
    while (cur) {
      if (comp(cur->GetValue(), value)) {
        best_node = cur;
        cur = cur->GetRight();
      } else {
        cur = cur->GetLeft();
      }
    }
    return best_node;
  }

  void Clear() {
    root_ = nullptr;
  }

  std::shared_ptr<SplayTreeNode<T>> GetRoot() {
    return root_;
  }
 private:
  void Splay(std::shared_ptr<SplayTreeNode<T>> node) {
    while (node->GetParent()) {
      auto p = node->GetParent();
      auto gp = p->GetParent();
      if (gp) {
        auto gg = gp->GetParent();
        bool is_left = true;
        if (gg && gp == gg->GetRight()) {
          is_left = false;
        }
        if (gp->GetLeft() == node->GetParent()) {
          if (node == node->GetParent()->GetLeft()) {
            if (p->GetRight()) {
              p->GetRight()->SetParent(gp);
            }
            gp->SetLeft(p->GetRight());
            p->SetRight(gp);
            if (node->GetRight()) {
              node->GetRight()->SetParent(p);
            }
            p->SetLeft(node->GetRight());
            node->SetRight(p);
            node->SetParent(gp->GetParent());
            p->SetParent(node);
            gp->SetParent(p);
            if (gp == root_) {
              root_ = node;
            }
          } else {
            if (node->GetRight()) {
              node->GetRight()->SetParent(gp);
            }
            gp->SetLeft(node->GetRight());
            if (node->GetLeft()) {
              node->GetLeft()->SetParent(p);
            }
            p->SetRight(node->GetLeft());
            node->SetLeft(p);
            node->SetRight(gp);
            node->SetParent(gp->GetParent());
            p->SetParent(node);
            gp->SetParent(node);
            if (gp == root_) {
              root_ = node;
            }
          }
        } else {
          if (node == node->GetParent()->GetLeft()) {
            if (node->GetLeft()) {
              node->GetLeft()->SetParent(gp);
            }
            gp->SetRight(node->GetLeft());
            if (node->GetRight()) {
              node->GetRight()->SetParent(p);
            }
            p->SetLeft(node->GetRight());
            node->SetLeft(gp);
            node->SetRight(p);
            node->SetParent(gp->GetParent());
            p->SetParent(node);
            gp->SetParent(node);
            if (gp == root_) {
              root_ = node;
            }
          } else {
            if (p->GetLeft()) {
              p->GetLeft()->SetParent(gp);
            }
            gp->SetRight(p->GetLeft());
            p->SetLeft(gp);
            if (node->GetLeft()) {
              node->GetLeft()->SetParent(p);
            }
            p->SetRight(node->GetLeft());
            node->SetLeft(p);
            node->SetParent(gp->GetParent());
            p->SetParent(node);
            gp->SetParent(p);
            if (gp == root_) {
              root_ = node;
            }
          }
        }
        if (gg) {
          if (is_left) {
            gg->SetLeft(node);
          } else {
            gg->SetRight(node);
          }
        }
      } else if (node == node->GetParent()->GetLeft()) {
        assert(root_ == node->GetParent());
        if (node->GetRight()) {
          node->GetRight()->SetParent(root_);
        }
        root_->SetLeft(node->GetRight());
        root_->SetParent(node);
        node->SetParent(nullptr);
        node->SetRight(root_);
        root_ = node;
      } else {
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
    }
  }

  Comp comp;
  std::shared_ptr<SplayTreeNode<T>> root_;
};

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
  int t;
  std::cin >> t;
  std::vector<RoomsRange> checkins;
  while (t--) {
    int type;
    std::cin >> type;
    if (type == 1) {
      int start, length;
      std::cin >> start >> length;
      checkins.push_back(RoomsRange(start, length, checkins.size()));
      RoomsRange cur_checkin(start + length, length, 0);
      auto p = splay_tree.GetLowerBound(cur_checkin);
      if (p && p->GetValue().GetRightmost() >= start) {
        std::cout << "0";
      } else {
        std::cout << "1";
        splay_tree.Insert(checkins.back());
      }
    } else {
      int checkin_id;
      std::cin >> checkin_id;
      auto p = splay_tree.Find(checkins[checkin_id]);
      if (p && p->GetValue().GetId() == checkin_id) {
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
