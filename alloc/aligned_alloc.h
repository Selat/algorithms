#pragma once

#include <cassert>
#include <cstddef>
#include <cstdlib>

template <typename T, size_t alignment>
class AlignedAlloc {
public:
  static_assert(alignment > 0);
  static_assert((alignment % sizeof(T)) == 0);
  static_assert(alignment < 256);
  typedef T value_type;

  void deallocate(T *p, size_t /* not used */) {
    assert(p != nullptr);
    uint8_t *aligned_ptr = reinterpret_cast<uint8_t*>(p);
    size_t shift = *(aligned_ptr - 1);
    uint8_t *ptr = aligned_ptr - shift;
    free(ptr);
  }

  T* allocate(size_t n) {
    uint8_t *ptr = static_cast<uint8_t*>(malloc(n * sizeof(T) + alignment));
    if (ptr == nullptr) {
      return nullptr;
    }
    size_t shift = alignment - (reinterpret_cast<size_t>(ptr) % alignment);
    uint8_t *aligned_ptr = ptr + shift;
    *(aligned_ptr - 1) = shift;
    return reinterpret_cast<T*>(aligned_ptr);
  }
};
