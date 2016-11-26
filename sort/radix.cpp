#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <chrono>
#include <queue>

std::vector<std::vector<int>> GenerateMatrix(
    int n, int a, int b, int c, int d) {
  std::vector<std::vector<int>> res(n, std::vector<int>(n));
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      res[i][j] = d;
      d = (d * a + b) % c;
    }
  }
  return res;
}

template <typename T>
std::vector<size_t> GetSortingPermutation(
    const std::vector<std::vector<T>>& matrix, int c) {
  std::vector<size_t> sorting_permutation(matrix.size());
  for (size_t i = 0; i < sorting_permutation.size(); ++i) {
    sorting_permutation[i] = i;
  }
  std::vector<std::queue<size_t>> queues(c);
  for (int component_id = matrix.size() - 1; component_id >= 0;
       --component_id) {
    for (size_t i = 0; i < sorting_permutation.size(); ++i) {
      size_t vector_id = sorting_permutation[i];
      queues[matrix[vector_id][component_id]].push(vector_id);
    }
    size_t cur_permutation_id = 0;
    for (size_t i = 0; i < queues.size(); ++i) {
      while (!queues[i].empty()) {
        sorting_permutation[cur_permutation_id++] = queues[i].front();
        queues[i].pop();
      }
    }
  }
  return sorting_permutation;
}

template <typename T>
std::vector<size_t> GetSortingPermutationSlow(
    const std::vector<std::vector<T>>& matrix) {
  std::vector<size_t> sorting_permutation(matrix.size());
  for (size_t i = 0; i < sorting_permutation.size(); ++i) {
    sorting_permutation[i] = i;
  }
  const size_t n = matrix[0].size();
  std::stable_sort(sorting_permutation.begin(), sorting_permutation.end(),
                   [&](size_t a, size_t b) {
    for (size_t i = 0; i < n; ++i) {
      if (matrix[a][i] < matrix[b][i]) {
        return true;
      } else if (matrix[a][i] > matrix[b][i]) {
        return false;
      }
    }
    return false;
  });
  return sorting_permutation;
}

void AssertPermutationsAreEqual(const std::vector<size_t> permutation1,
                                const std::vector<size_t> permutation2) {
  assert(permutation1.size() == permutation2.size());
  for (size_t i = 0; i < permutation1.size(); ++i) {
    assert(permutation1[i] == permutation2[i]);
  }
}

void RunTests() {
  const auto time_now = []() {
    return std::chrono::high_resolution_clock::now();
  };
  auto matrix = GenerateMatrix(10000, 1, 2, 3, 1);
  auto begin1 = time_now();
  auto p1 = GetSortingPermutation(matrix, 3);
  auto end1 = time_now();
  auto begin2 = time_now();
  auto p2 = GetSortingPermutationSlow(matrix);
  auto end2 = time_now();

  AssertPermutationsAreEqual(p1, p2);

  std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
      end1 - begin1).count() << " ms" << std::endl;
  std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
      end2 - begin2).count() << " ms" << std::endl;
}

int main() {
  // RunTests();
  int n, a, b, c, d_0;
  std::cin >> n >> a >> b >> c >> d_0;
  auto matrix = GenerateMatrix(n, a, b, c, d_0);
  auto sorting_permutation = GetSortingPermutation(matrix, c);
  for (int id : sorting_permutation) {
    std::cout << id << " ";
  }
  std::cout << std::endl;
  return 0;
}
