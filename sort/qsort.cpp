#include <algorithm>
#include <chrono>
#include <climits>
#include <cassert>
#include <functional>
#include <iostream>
#include <random>

template <typename RandomAccessIterator>
void QuickSort(RandomAccessIterator begin,
               RandomAccessIterator end,
               std::function<int(int)>& rn_generator) {
  while (begin + 1 < end) {
    // For convenience pivot is moved to the beginning
    int pivot_id = rn_generator(end - begin);
    std::swap(*begin, *(begin + pivot_id));
    const auto pivot = *begin;
    RandomAccessIterator lt = begin, i = begin, gt = end - 1;
    // Loop invariants:
    // [begin..lt) < pivot
    // [lt..i) == pivot
    // [i..gt] - not processed yet
    // (gt..end) > pivot
    while (i <= gt) {
      if (*i < pivot) {
        std::swap(*i, *lt);
        ++lt;
        ++i;
      } else if (*i > pivot) {
        std::swap(*i, *gt);
        --gt;
      } else {
        ++i;
      }
    }

    if (end - gt > lt - begin) {
      QuickSort(begin, lt, rn_generator);
      begin = gt;
    } else {
      QuickSort(gt, end, rn_generator);
      end = lt;
    }
  }
}

template <typename RandomAccessIterator>
void QuickSort(RandomAccessIterator begin,
               RandomAccessIterator end) {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator(seed);
  // Generates an integer uniformely distributed in range [0..n - 1]
  std::function<int(int)> rn_generator = [&generator](int n) -> int {
    std::uniform_int_distribution<int> distribution(0, n - 1);
    return distribution(generator);
  };
  QuickSort(begin, end, rn_generator);
}

void RunTests() {
  constexpr int n = 1000 * 1000;
  std::vector<int> a(n);
  std::default_random_engine generator(
      std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<int> distribution(INT_MIN, INT_MAX);
  for (int i = 0; i < n; ++i) {
    a[i] = distribution(generator);
  }
  std::vector<int> a_reference(a);

  const auto time_now = []() {
    return std::chrono::high_resolution_clock::now();
  };

  auto start1 = time_now();
  QuickSort(a.begin(), a.end());
  auto end1 = time_now();
  auto start2 = time_now();
  std::sort(a_reference.begin(), a_reference.end());
  auto end2 = time_now();
  for (int i = 0; i < n; ++i) {
    assert(a[i] == a_reference[i]);
  }

  std::cout << "QuickSort: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   end1 - start1).count()
            << "ms" << std::endl;
  std::cout << "std::sort: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   end2 - start2).count()
            << "ms" << std::endl;
}

int main() {
  RunTests();
  return 0;
}
