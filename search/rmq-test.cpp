#include <iostream>
#include <vector>
#include <cassert>
#include <chrono>

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
  SparseTable<int> rmq;
  rmq.Init(data);
  for (int i = 0; i < kTestsNum; ++i) {
    size_t l = rand() % (n - 1);
    size_t r = l + (rand() % (n - l)) + 1;
    auto rmq_ans = rmq.QueryMin(l, r, data);
    assert(rmq_ans >= l && rmq_ans < r);
    auto brute_ans = FindMins(data, l, r);
    assert(brute_ans[0] == data[rmq_ans]);
  }
}

void RunSpeedTests() {
  constexpr int n = 10000000;
  constexpr int kTestsNum = 10000000;
  std::vector<int> data(n);
  for (size_t i = 0; i < data.size(); ++i) {
    data[i] = rand();
  }
  const auto time_now = []() {
    return std::chrono::high_resolution_clock::now();
  };
  auto start1 = time_now();
  RmqLca<int> rmq;
  rmq.Init(data);
  auto end1 = time_now();
  std::cout << "Fast-RMQ init time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count() << " ms." << std::endl;
  auto start = time_now();
  double total_time = 0;
  for (int i = 0; i < kTestsNum; ++i) {
    size_t l = rand() % (n - 1);
    size_t r = l + (rand() % (n - l)) + 1;
    auto rmq_ans = rmq.QueryMin(l, r);
    assert(rmq_ans >= l && rmq_ans < r);
  }
  auto end = time_now();
  total_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  std::cout << "Fast-RMQ average query time: " << total_time / kTestsNum * 1000 << " us." << std::endl;

  auto start2 = time_now();
  SparseTable<int> st;
  st.Init(data);
  auto end2 = time_now();
  std::cout << "SparseTable Init time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count() << " ms." << std::endl;
  auto start3 = time_now();
  total_time = 0.0;
  for (int i = 0; i < kTestsNum; ++i) {
    size_t l = rand() % (n - 1);
    size_t r = l + (rand() % (n - l)) + 1;
    auto st_ans = st.QueryMin(l, r, data);
    assert(st_ans >= l && st_ans < r);
  }
  auto end3 = time_now();
  total_time += std::chrono::duration_cast<std::chrono::milliseconds>(end3 - start3).count();
  std::cout << "SparseTable average query time: " << total_time / kTestsNum * 1000 << " us." << std::endl;
}

void RunTests2() {
  constexpr int n = 1000;
  constexpr int kTestsNum = 10000;
  std::vector<int> data(n);
  for (size_t i = 0; i < data.size(); ++i) {
    data[i] = rand();
  }
  RmqLca<int> rmq;
  rmq.Init(data);
  SparseTable<int> st;
  st.Init(data);
  for (int i = 0; i < kTestsNum; ++i) {
    size_t l = rand() % (n - 1);
    size_t r = l + (rand() % (n - l)) + 1;
    auto rmq_ans = rmq.Query(l, r, data);
    if (l + 1 == r) {
      assert(rmq_ans.size() == 1 && rmq_ans[0] == l);
    } else {
      assert(rmq_ans.size() == 2);
      auto brute_ans = FindMins(data, l, r);
      for (size_t j = 0; j < rmq_ans.size(); ++j) {
        assert(data[rmq_ans[j]] == brute_ans[j]);
      }
    }
  }
}

int main() {
  RunTests();
  RunTests2();
  RunSpeedTests();
  std::cout << "All tests passed!" << std::endl;
  return 0;
}

// Results:
// Fast-RMQ init time: 1583 ms.
// Fast-RMQ average query time: 0.4136 us.
// SparseTable init time: 26842 ms.
// SparseTable average query time: 0.1562 us.
// All tests passed!
