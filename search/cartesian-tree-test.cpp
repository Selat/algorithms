#include <cassert>
#include <vector>
#include <memory>
#include <sstream>
#include <iostream>
#include <chrono>

#include "cartesian-tree.hpp"
#include "cartesian-tree-array.hpp"

template <typename T>
void AssertVectorEqual(const std::vector<T>& v1, const std::vector<T>& v2) {
  assert(v1.size() == v2.size());
  for (int i = 0; i < v1.size(); ++i) {
    assert(v1[i] == v2[i]);
  }
}

void RunTests() {
  constexpr int n = 1000000;
  std::vector<int> data(n);
  for (size_t i = 0; i < data.size(); ++i) {
    data[i] = rand();
    // std::cout << data[i] << " ";
  }
  // std::cout << std::endl;
  const auto time_now = []() {
    return std::chrono::high_resolution_clock::now();
  };
  auto start1 = time_now();
  auto cartesian_tree = CartesianTree<int>::Init(data);
  auto end1 = time_now();
  cartesian_tree.CheckHeapProperty();
  std::vector<int> tmp;
  cartesian_tree.GetRoot()->Inorder(tmp);
  AssertVectorEqual(data, tmp);

  CartesianTreeArray<int> cartesian_tree_array;
  auto start2 = time_now();
  cartesian_tree_array.Init(data);
  auto end2 = time_now();
  cartesian_tree_array.CheckHeapProperty();
  tmp.clear();
  cartesian_tree_array.Dfs(tmp);
  AssertVectorEqual(data, tmp);

  std::cout << "CartesianTree: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   end1 - start1).count()
            << "ms" << std::endl;
  std::cout << "CartesianTreeArray: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   end2 - start2).count()
            << "ms" << std::endl;

  std::cout << "All tests passed!" << std::endl;
}

int main() {
  RunTests();
  return 0;
}
