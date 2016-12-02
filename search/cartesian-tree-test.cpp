#include <cassert>
#include <vector>
#include <memory>
#include <sstream>
#include <iostream>

#include "cartesian-tree.hpp"

template <typename T>
void AssertVectorEqual(const std::vector<T>& v1, const std::vector<T>& v2) {
  assert(v1.size() == v2.size());
  for (int i = 0; i < v1.size(); ++i) {
    assert(v1[i] == v2[i]);
  }
}

void RunTests() {
  constexpr int n = 1000;
  constexpr int kTestsNum = 10000;
  std::vector<int> data(n);
  for (size_t i = 0; i < data.size(); ++i) {
    data[i] = rand();
  }
  auto cartesian_tree = CartesianTree<int>::Init(data);
  cartesian_tree.CheckHeapProperty();
  std::vector<int> tmp;
  cartesian_tree.GetRoot()->Inorder(tmp);
  AssertVectorEqual(data, tmp);
  std::cout << "All tests passed!" << std::endl;
}

int main() {
  RunTests();
  return 0;
}
