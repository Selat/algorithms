#include <iostream>
#include <stack>
#include <vector>
#include <random>
#include <chrono>
#include <climits>
#include <algorithm>
#include <cassert>
#include <type_traits>

constexpr int kMaxArraySizeForInsertionSort = 10;

template <typename RandomAccessIterator>
void InsertionSort(RandomAccessIterator begin, RandomAccessIterator end) {
  for (RandomAccessIterator it = begin + 1; it != end; ++it) {
    RandomAccessIterator cur = it;
    while (cur != begin && *cur < *(cur - 1)) {
      std::swap(*cur, *(cur - 1));
      --cur;
    }
  }
}

// Merges two sorted arrays [begin1; end1) and [begin2; end2). The result is
// stored in |buffer_begin|. Content of |buffer_begin| is moved to input arrays.
template <typename RandomAccessIterator>
void Merge(RandomAccessIterator begin1, RandomAccessIterator end1,
           RandomAccessIterator begin2, RandomAccessIterator end2,
           RandomAccessIterator buffer_begin) {
  while (begin1 != end1 && begin2 != end2) {
    if (*begin1 < *begin2) {
      std::swap(*buffer_begin, *begin1);
      ++begin1;
    } else {
      std::swap(*buffer_begin, *begin2);
      ++begin2;
    }
    ++buffer_begin;
  }
  while (begin1 != end1) {
    std::swap(*(buffer_begin++), *(begin1++));
  }
  while (begin2 != end2) {
    std::swap(*(buffer_begin++), *(begin2++));
  }
}

// Swaps [begin; end) and [out; out + (end - begin)) buffers content.
template <typename RandomAccessIterator>
void Swap(RandomAccessIterator begin, RandomAccessIterator end,
          RandomAccessIterator out) {
  while (begin != end) {
    std::swap(*begin, *out);
    ++begin;
    ++out;
  }
}

// Sorts an array using external buffer for merging.
// Elements are swapped instead of being overwritten, so the original buffer's
// content is preserved.
template <typename RandomAccessIterator>
void MergeSort(RandomAccessIterator begin, RandomAccessIterator end,
               RandomAccessIterator buffer_begin) {
  size_t cur_size = kMaxArraySizeForInsertionSort;
  for (auto it = begin; it < end; it += cur_size) {
    InsertionSort(it, it + cur_size);
  }
  while (cur_size < end - begin) {
    for (auto it = begin; it + cur_size < end; it += 2 * cur_size) {
      auto begin1 = it;
      auto end1 = it + cur_size;
      auto begin2 = end1;
      auto end2 = (begin2 + cur_size < end)?begin2 + cur_size:end;
      Merge(begin1, end1, begin2, end2, buffer_begin + (it - begin));
      Swap(buffer_begin + (it - begin), buffer_begin + (end2 - begin), it);
    }
    cur_size *= 2;
  }
}

// Sorts an array using O(1) extra memory.
template <typename RandomAccessIterator>
void MergeSort(RandomAccessIterator begin, RandomAccessIterator end) {
  if (begin >= end) {
    return;
  }
  RandomAccessIterator sorted_begin = (end - begin + 1) / 2 + begin;
  MergeSort(sorted_begin, end, begin);
  while (sorted_begin != begin) {
    if (sorted_begin - begin <= kMaxArraySizeForInsertionSort) {
      InsertionSort(begin, sorted_begin);
      std::vector<typename std::remove_reference<decltype(*begin)>::type>
          buffer(begin, sorted_begin);
      Merge(buffer.begin(), buffer.end(), sorted_begin, end, begin);
      sorted_begin = begin;
    } else {
      size_t pending_sort_size = (sorted_begin - begin) / 2;
      RandomAccessIterator pending_sort_end = begin + pending_sort_size;
      MergeSort(begin, pending_sort_end, pending_sort_end);
      Merge(begin, pending_sort_end, sorted_begin, end,
            sorted_begin - pending_sort_size);
      sorted_begin = sorted_begin - pending_sort_size;
    }
  }
}

std::vector<int> GenerateRandomArray(const int n) {
  std::vector<int> a(n);
  std::default_random_engine generator(
      std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<int> distribution(INT_MIN, INT_MAX);
  for (int i = 0; i < n; ++i) {
    a[i] = distribution(generator);
  }
  return a;
}

void TestExternalMergeSort() {
  constexpr int n = 1000 * 1000;
  std::vector<int> a = GenerateRandomArray(n);
  std::vector<int> a_reference(a);
  std::vector<int> buffer(n);

  const auto time_now = []() {
    return std::chrono::high_resolution_clock::now();
  };

  auto start1 = time_now();
  MergeSort(a.begin(), a.end(), buffer.begin());
  auto end1 = time_now();
  auto start2 = time_now();
  std::sort(a_reference.begin(), a_reference.end());
  auto end2 = time_now();
  for (int i = 0; i < n; ++i) {
    assert(a[i] == a_reference[i]);
  }

  std::cout << "External MergeSort: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   end1 - start1).count()
            << "ms" << std::endl;
  std::cout << "std::sort: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   end2 - start2).count()
            << "ms" << std::endl;
}

void TestInPlaceMergeSort() {
  constexpr int n = 1000 * 1000;
  std::vector<int> a = GenerateRandomArray(n);
  std::vector<int> a_reference(a);
  std::vector<int> buffer(n);

  const auto time_now = []() {
    return std::chrono::high_resolution_clock::now();
  };

  auto start1 = time_now();
  MergeSort(a.begin(), a.end());
  auto end1 = time_now();
  auto start2 = time_now();
  std::sort(a_reference.begin(), a_reference.end());
  auto end2 = time_now();
  for (int i = 0; i < n; ++i) {
    assert(a[i] == a_reference[i]);
  }

  std::cout << "In-place MergeSort: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   end1 - start1).count()
            << "ms" << std::endl;
  std::cout << "std::sort: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   end2 - start2).count()
            << "ms" << std::endl;
}

int main() {
  TestExternalMergeSort();
  // External MergeSort: 102ms
  // std::sort: 84ms
  TestInPlaceMergeSort();
  // In-place MergeSort: 110ms
  // std::sort: 84ms
  return 0;
}
