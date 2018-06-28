#pragma once

#include <cassert>

// This class provides interface for indexing matricies represented as
// continuous in-memory blobs with specified number of rows, columns and
// bytes per row.
template <typename T>
class BlobMatrix2D {
 public:
  BlobMatrix2D(void *data, size_t rows, size_t cols, size_t bytes_per_row = 0)
      : data_(static_cast<uint8_t*>(data)), rows_(rows), cols_(cols),
        bytes_per_row_(bytes_per_row) {
    if (bytes_per_row_ == 0) {
      bytes_per_row_ = cols_ * sizeof(T);
    } else {
      assert(bytes_per_row_ >= cols_ * sizeof(T));
    }
  }
  BlobMatrix2D(const BlobMatrix2D&) = delete;

  T& operator()(int i, int j) {
    return *(reinterpret_cast<T*>(data_ + i * bytes_per_row_) + j);
  }
 private:
  uint8_t *data_;
  size_t rows_;
  size_t cols_;
  size_t bytes_per_row_;
};
