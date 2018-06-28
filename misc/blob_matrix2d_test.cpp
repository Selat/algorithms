#include "blob_matrix2d.h"

#include <iostream>

void TestCompactPacking() {
  constexpr int kWidth = 100;
  constexpr int kHeight = 200;
  int *data = new int[kWidth * kHeight];
  for (int i = 0; i < kHeight; ++i) {
    for (int j = 0; j < kWidth; ++j) {
      data[i * kWidth + j] = i * kWidth + j;
    }
  }

  BlobMatrix2D<int> blob_matrix(data, kHeight, kWidth);
  for (int i = 0; i < kHeight; ++i) {
    for (int j = 0; j < kWidth; ++j) {
      assert(blob_matrix(i, j) == i * kWidth + j);
    }
  }

  delete[] data;
}

void TestRowPadding() {
  constexpr int kWidth = 100;
  constexpr int kHeight = 200;
  constexpr int kBytesPerRow = kWidth * 5 + 64;
  int *data = new int[kBytesPerRow / sizeof(int) * kHeight];
  for (int i = 0; i < kHeight; ++i) {
    for (int j = 0; j < kWidth; ++j) {
      data[i * kBytesPerRow / sizeof(int) + j] = i * kWidth + j;
    }
  }

  BlobMatrix2D<int> blob_matrix(data, kHeight, kWidth, kBytesPerRow);
  for (int i = 0; i < kHeight; ++i) {
    for (int j = 0; j < kWidth; ++j) {
      assert(blob_matrix(i, j) == i * kWidth + j);
    }
  }

  delete[] data;
}

int main() {
  TestCompactPacking();
  TestRowPadding();
  std::cout << "All tests passed! :)" << std::endl;
  return 0;
}
