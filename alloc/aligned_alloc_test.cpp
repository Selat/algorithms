#include "aligned_alloc.h"

#include <iostream>
#include <vector>

void TestLinearFill(int n) {
  std::cout << "Testing linear vector fill..." << std::flush;

  std::vector<int, AlignedAlloc<int, sizeof(int)>> data;
  assert(((reinterpret_cast<uint64_t>(data.data())) % sizeof(int)) == 0);
  for (int i = 0; i < n; ++i) {
    data.push_back(i);
  }
  for (int i = 0; i < n; ++i) {
    assert(data[i] == i);
  }

  std::cout << "ok!" << std::endl;
}

void TestAddressAlignments(int tests_num) {
  std::cout << "Testing address alignements..." << std::flush;

  for (int i = 0; i < tests_num; ++i) {
    std::vector<int, AlignedAlloc<int, 16>> data16;
    data16.push_back(0);
    assert(((reinterpret_cast<uint64_t>(data16.data())) % 16) == 0);

    std::vector<int, AlignedAlloc<int, 32>> data32;
    data32.push_back(0);
    assert(((reinterpret_cast<uint64_t>(data32.data())) % 32) == 0);

    std::vector<int, AlignedAlloc<int, 64>> data64;
    data64.push_back(0);
    assert(((reinterpret_cast<uint64_t>(data64.data())) % 64) == 0);

    std::vector<int, AlignedAlloc<int, 128>> data128;
    data128.push_back(0);
    assert(((reinterpret_cast<uint64_t>(data128.data())) % 128) == 0);
  }

  std::cout << "ok!" << std::endl;
}

int main() {
  TestLinearFill(100000);
  TestAddressAlignments(1000);
  return 0;
}
