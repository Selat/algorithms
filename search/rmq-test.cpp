#include <iostream>
#include <vector>
#include <cassert>

#include "sparse-table.hpp"

template <typename T>
T FindMin(const std::vector<T>& data, int l, int r) {
  T res = std::numeric_limits<T>::max();
  for (int i = l; i < r; ++i) {
    res = std::min(res, data[i]);
  }
  return res;
}

void RunTests() {
  constexpr int n = 10000;
  constexpr int kTestsNum = 10000;
  std::vector<int> data(n);
  for (size_t i = 0; i < data.size(); ++i) {
    data[i] = rand();
  }
  auto rmq = SparseTable<int>::Init(data);
  for (int i = 0; i < kTestsNum; ++i) {
    int l = rand() % n;
    int r = (rand() % n) + 1;
    if (l > r) {
      std::swap(l, r);
    } else if (l == r) {
      ++r;
    }
    int rmq_ans = rmq.Query(l, r);
    int brute_ans = FindMin(data, l, r);
    assert(rmq_ans == brute_ans);
  }

  std::cout << "All tests passed!" << std::endl;
}

int main() {
  RunTests();
  return 0;
}
