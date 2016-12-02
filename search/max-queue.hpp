#ifndef MAX_QUEUE_HPP
#define MAX_QUEUE_HPP

#include <algorithm>
#include <cassert>

constexpr int kVectorExpansionCoefficient = 2;

template <typename T>
class Vector {
 public:
  Vector();
  ~Vector();
  void PushBack(const T& value);
  void PopBack();
  // Reference isn't guaranteed to remain valid after any modification
  // operation.
  const T& operator[](size_t id) const;
  const T& PeekBack() const;
  bool Empty() const;
 private:
  T* data_;
  size_t size_;
  size_t allocated_size_;
};

template <typename T>
class Stack {
 public:
  void PushBack(const T& value);
  void PopBack();
  const T& PeekBack() const;
  bool Empty() const;
 private:
  Vector<T> data_;
};

template <typename T>
class StackWithMaximum {
 public:
  void PushBack(const T& value);
  void PopBack();
  const T& PeekMax() const;
  const T& PeekBack() const;
  bool Empty() const;
 private:
  Stack<T> data_;
  Stack<T> maximums_;
};

template <typename T>
class QueueWithMaximum {
 public:
  void PushBack(const T& value);
  void PopFront();
  const T& PeekFront();
  const T& PeekMax();
 private:
  void PrepareQueueOrderData();
  StackWithMaximum<T> data_queue_order_;
  StackWithMaximum<T> data_reversed_queue_order_;
};

template <typename T>
Vector<T>::Vector() {
  allocated_size_ = 1;
  data_ = new T[allocated_size_];
  size_ = 0;
}

template <typename T>
Vector<T>::~Vector() {
  delete[] data_;
}

template <typename T>
void Vector<T>::PushBack(const T& value) {
  if (size_ == allocated_size_) {
    // We have store a local copy of |value| in order to prevent
    // use-after-free error which occurs when |value| contains a reference
    // to variable stored in |data_| array.
    // For example: v.PushBack(v.PeekBack())
    T tmp_value = value;
    allocated_size_ = allocated_size_ * kVectorExpansionCoefficient;
    assert(allocated_size_ > size_);

    T* new_data = new T[allocated_size_];
    std::copy(data_, data_ + size_, new_data);
    delete[] data_;
    data_ = new_data;
    data_[size_++] = tmp_value;
  } else {
    data_[size_++] = value;
  }
}

template <typename T>
void Vector<T>::PopBack() {
  assert(size_ > 0);
  --size_;
}

template <typename T>
const T& Vector<T>::operator[](size_t id) const {
  assert(id < size_);
  return data_[id];
}

template <typename T>
const T& Vector<T>::PeekBack() const {
  assert(size_ > 0);
  return data_[size_ - 1];
}

template <typename T>
bool Vector<T>::Empty() const {
  return size_ == 0;
}

template <typename T>
void Stack<T>::PushBack(const T& value) {
  data_.PushBack(value);
}

template <typename T>
void Stack<T>::PopBack() {
  data_.PopBack();
}

template <typename T>
const T& Stack<T>::PeekBack() const {
  return data_.PeekBack();
}

template <typename T>
bool Stack<T>::Empty() const {
  return data_.Empty();
}

template <typename T>
void StackWithMaximum<T>::PushBack(const T& value) {
  data_.PushBack(value);
  if (!maximums_.Empty()) {
    maximums_.PushBack(std::max(maximums_.PeekBack(), value));
  } else {
    maximums_.PushBack(value);
  }
}

template <typename T>
void StackWithMaximum<T>::PopBack() {
  data_.PopBack();
  maximums_.PopBack();
}

template <typename T>
const T& StackWithMaximum<T>::PeekBack() const {
  return data_.PeekBack();
}

template <typename T>
const T& StackWithMaximum<T>::PeekMax() const {
  return maximums_.PeekBack();
}

template <typename T>
bool StackWithMaximum<T>::Empty() const {
  return data_.Empty();
}

template <typename T>
void QueueWithMaximum<T>::PushBack(const T& value) {
  data_reversed_queue_order_.PushBack(value);
}

template <typename T>
void QueueWithMaximum<T>::PopFront() {
  if (data_queue_order_.Empty()) {
    PrepareQueueOrderData();
  }
  data_queue_order_.PopBack();
}

template <typename T>
const T& QueueWithMaximum<T>::PeekFront() {
  if (data_queue_order_.Empty()) {
    PrepareQueueOrderData();
  }
  return data_queue_order_.PeekBack();
}

template <typename T>
const T& QueueWithMaximum<T>::PeekMax() {
  assert(!data_queue_order_.Empty() || !data_reversed_queue_order_.Empty());
  if (data_queue_order_.Empty()) {
    PrepareQueueOrderData();
    return data_queue_order_.PeekMax();
  } else if (!data_reversed_queue_order_.Empty()) {
    return std::max(data_queue_order_.PeekMax(),
                    data_reversed_queue_order_.PeekMax());
  } else {
    return data_queue_order_.PeekMax();
  }
}

template <typename T>
void QueueWithMaximum<T>::PrepareQueueOrderData() {
  assert(data_queue_order_.Empty());
  while (!data_reversed_queue_order_.Empty()) {
    data_queue_order_.PushBack(data_reversed_queue_order_.PeekBack());
    data_reversed_queue_order_.PopBack();
  }
}

#endif  // MAX_QUEUE_HPP
