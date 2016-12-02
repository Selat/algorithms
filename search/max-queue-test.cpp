#include <algorithm>
#include <iostream>
#include <climits>

#include "max-queue.hpp"

int main() {
  int n, w;
  std::cin >> n >> w;
  QueueWithMaximum<int> q;
  long long cur_fence_area = 0;
  long long ans = LONG_LONG_MAX;
  for (int i = 0; i < n; ++i) {
    int cur_height;
    std::cin >> cur_height;
    cur_fence_area += cur_height;
    q.PushBack(cur_height);
    if (i >= w) {
      cur_fence_area -= q.PeekFront();
      q.PopFront();
    }
    if (i >= w - 1) {
      ans = std::min(ans, static_cast<long long>(q.PeekMax()) * w -
                          cur_fence_area);
    }
  }
  ans = std::min(ans, static_cast<long long>(q.PeekMax()) * w - cur_fence_area);
  std::cout << ans << std::endl;
  return 0;
}
