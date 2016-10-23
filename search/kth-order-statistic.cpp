#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <random>
#include <climits>
#include <algorithm>
#include <cassert>
#include <type_traits>

template <typename RandomAccessIterator>
void Print(RandomAccessIterator begin, RandomAccessIterator end) {
  for (auto it = begin; it != end; ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;
}

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

template <typename RandomAccessIterator>
RandomAccessIterator OrderStatistic(RandomAccessIterator begin,
                                    RandomAccessIterator end, int k) {
  assert(k >= 0);
  assert(k < end - begin);
  while (end - begin > 5) {
    std::vector<typename std::remove_reference<decltype(*begin)>::type> meds;
    // We should ignore the last non-complete chunk
    for (RandomAccessIterator it = begin; it + 5 <= end; it += 5) {
      InsertionSort(it, it + 5);
      meds.push_back(*(it + 2));
    }
    auto meds_pivot_iterator = OrderStatistic(
        meds.begin(), meds.end(), meds.size() / 2);

    // Compute pivot position in the original array
    int chunk_id = meds_pivot_iterator - meds.begin();
    RandomAccessIterator pivot_iterator = begin + (chunk_id * 5 + 2);

    // For convenience pivot is moved to the beginning
    std::swap(*begin, *pivot_iterator);
    const auto pivot = *begin;

    // Loop invariants:
    // [begin..lt) < pivot
    // [lt..i) == pivot
    // [i..gt] - not processed yet
    // (gt..end) > pivot
    auto lt = begin, i = begin, gt = end - 1;
    while (i <= gt) {
      if (*i < pivot) {
        std::swap(*i, *lt);
        ++i;
        ++lt;
      } else if (*i > pivot) {
        std::swap(*i, *gt);
        --gt;
      } else {
        ++i;
      }
    }

    if (begin + k >= lt && begin + k <= gt) {
      return lt;
    } else if (begin + k < lt) {
      end = lt;
    } else {
      k -= gt - begin + 1;
      begin = gt + 1;
    }
  }

  assert(k >= 0);
  assert(k < end - begin);
  assert(begin < end);
  InsertionSort(begin, end);
  return begin + k;
}

void TestInsertionSort() {
  constexpr int n = 1000;
  std::vector<int> a(n);
  std::default_random_engine generator(
      std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<int> distribution(INT_MIN, INT_MAX);
  for (int i = 0; i < n; ++i) {
    a[i] = distribution(generator);
  }
  std::vector<int> a_reference(a);

  InsertionSort(a.begin(), a.end());
  std::sort(a_reference.begin(), a_reference.end());

  for (int i = 0; i < n; ++i) {
    assert(a[i] == a_reference[i]);
  }
}

void TestOrderStatistic() {
  constexpr int n = 1000 * 1000;
  constexpr int kTestsNum = 10;
  std::vector<int> a(n);
  std::default_random_engine generator(
      std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<int> elements_distribution(INT_MIN, INT_MAX);
  for (int i = 0; i < n; ++i) {
    a[i] = elements_distribution(generator);
  }
  std::vector<int> a_reference(a);
  std::sort(a_reference.begin(), a_reference.end());

  std::uniform_int_distribution<int> indicies_distribution(0, n - 1);
  for (int i = 0; i < kTestsNum; ++i) {
    int id = indicies_distribution(generator);
    int val = *OrderStatistic(a.begin(), a.end(), id);
    assert(val == a_reference[id]);
  }
}

int main() {
  TestInsertionSort();
  TestOrderStatistic();
  int tests_num;
  std::cin >> tests_num;
  while (tests_num--) {
    int n, k;
    std::cin >> n >> k;
    --k;
    std::vector<int> data(n);
    for (int i = 0; i < n; ++i) {
      std::cin >> data[i];
    }
    std::cout << *OrderStatistic(data.begin(), data.end(), k) << std::endl;
  }
  return 0;
}
