#include <iostream>
#include <vector>
#include <cassert>

#include "sparse-table.hpp"
#include "lca-rmq.hpp"

template <typename T>
std::vector<T> FindMins(const std::vector<T>& data, int l, int r) {
  T min1 = std::numeric_limits<T>::max();
  T min2 = std::numeric_limits<T>::max();
  for (int i = l; i < r; ++i) {
    if (data[i] < min1) {
      min2 = min1;
      min1 = data[i];
    } else if (data[i] < min2) {
      min2 = data[i];
    }
  }
  return {min1, min2};
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
    size_t l = rand() % (n - 1);
    size_t r = l + (rand() % (n - l)) + 1;
    auto rmq_ans = rmq.Query(l, r);
    if (rmq_ans.size() == 2) {
      assert(rmq_ans[0] >= 0 && rmq_ans[0] < n);
      assert(rmq_ans[1] >= 0 && rmq_ans[1] < n);
      auto brute_ans = FindMins(data, l, r);
      assert(data[rmq_ans[0]] == brute_ans[0] && data[rmq_ans[1]] == brute_ans[1]);
    } else {
      assert(rmq_ans.size() == 1 && r - l == 1);
      assert(rmq_ans[0] == l);
    }
  }

  std::cout << "All tests passed!" << std::endl;
}

void RunTests2() {
  constexpr int n = 10000;
  constexpr int kTestsNum = 10000;
  std::vector<int> data(n);
  for (size_t i = 0; i < data.size(); ++i) {
    data[i] = rand();
  }
  auto rmq = RmqLca<int>::Init(data);
  for (int i = 0; i < kTestsNum; ++i) {
    int l = rand() % n;
    int r = (rand() % n) + 1;
    if (l > r) {
      std::swap(l, r);
    } else if (l == r) {
      ++r;
    }
    // std::cout << i << " " << l << " " << r << std::endl;
    // int rmq_ans = rmq.Query(l, r);
    // assert(rmq_ans >= 0 && rmq_ans < n);
    // int brute_ans = FindMin(data, l, r);
    // assert(data[rmq_ans] == brute_ans);
    // std::cout << "--------------------------" << std::endl;
  }

  std::cout << "All tests passed!" << std::endl;
}

int main() {
  RunTests();
  // RunTests2();
  return 0;
}
