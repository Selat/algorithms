#include <thread>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include <cassert>
#include <chrono>

template <typename Iterator, typename T>
void accumulateChunk(Iterator first, Iterator last, T& res) {
  res = std::accumulate(first, last, res);
}

template <typename Iterator, typename T>
T accumulate(Iterator first, Iterator last, T init) {
  const size_t kSize = std::distance(first, last);
  if (!kSize) return init;
  const size_t kMinPerThread = 10;
  const size_t kChunksNum = (kSize + kMinPerThread - 1) / kMinPerThread;
  const size_t kHardwareConcurrency = std::thread::hardware_concurrency();
  const size_t kThreadsNum = std::min(
      kChunksNum, kHardwareConcurrency?kHardwareConcurrency:0);
  const size_t kChunkSize = kSize / kThreadsNum;
  Iterator block_start = first;
  std::vector<std::thread> threads;
  std::vector<T> results(kThreadsNum);
  for (size_t thread_id = 0; thread_id + 1 < kThreadsNum; ++thread_id) {
    Iterator block_end = block_start;
    std::advance(block_end, kChunkSize);
    threads.push_back(std::thread(accumulateChunk<Iterator,T>, block_start,
                                  block_end, std::ref(results[thread_id])));
    block_start = block_end;
  }
  accumulateChunk(block_start, last, results[kThreadsNum - 1]);
  for (auto& thread : threads) {
    thread.join();
  }
  return std::accumulate(results.begin(), results.end(), init);
}

int main() {
  constexpr size_t kSize = 40 * 1000 * 1000;
  std::vector<unsigned long long> data(kSize);
  for (size_t i = 0; i < data.size(); ++i) {
    data[i] = i + 1;
  }
  const auto time_now = [](){return std::chrono::high_resolution_clock::now();};
  const auto start1 = time_now();
  unsigned long long sum = ::accumulate(data.begin(), data.end(), 0ull);
  const auto end1 = time_now();
  std::cout << "Parallel: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                end1 - start1).count() << "ms" << std::endl;
  const auto start2 = time_now();
  std::accumulate(data.begin(), data.end(), 0ull);
  const auto end2 = time_now();
  std::cout << "Sequential: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                end2 - start2).count() << "ms" << std::endl;
  constexpr unsigned long long kExpectedSum =
      static_cast<unsigned long long>(kSize) * (kSize + 1) / 2ull;
  assert(kExpectedSum == sum);
  return 0;
}
