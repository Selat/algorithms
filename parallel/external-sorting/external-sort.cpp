#include <algorithm>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <chrono>
#include <fstream>
#include <vector>
#include <set>

class Timer {
public:
  Timer() {
    start();
  }
  void start() {
    start_ = std::chrono::high_resolution_clock::now();
  }
  void stop() {
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Elapsed time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     end - start_).count()
              << "ms" << std::endl;
  }
private:
  std::chrono::high_resolution_clock::time_point start_;
};

bool IsMasterProcess(int rank) {
  return rank == 0;
}

constexpr int kChunkSize = 10 * 1000 * 1000;

int main(int argc, char ** argv) {
  int rank;
  int processes_num = 4;
  Timer timer;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &processes_num);

  if (processes_num == 1) {
    MPI_Finalize();
    std::cerr << "Please use at least 2 processes" << std::endl;
    return -1;
  }

  MPI_Barrier(MPI_COMM_WORLD);

	if (IsMasterProcess(rank)) {
		printf("Sorting with %d processes...\n", (int)processes_num);
    std::vector<int> a(kChunkSize);
    int n;
    std::ifstream in("array.txt");
    in >> n;
    int cur_process_id = 1;
    int cur_chunk_id = 0;
    int params[2];
    for (int i = 0; i < n; i += kChunkSize) {
      int cur_size = std::min(n - i, kChunkSize);
      params[0] = cur_size;
      params[1] = cur_chunk_id;
      for (int j = 0; j < cur_size; ++j) {
        in >> a[j + 2];
      }
      MPI_Send(params, 2, MPI_INT, cur_process_id, 0, MPI_COMM_WORLD);
      MPI_Send(&a[0], cur_size, MPI_INT, cur_process_id, 0, MPI_COMM_WORLD);
      ++cur_chunk_id;
      ++cur_process_id;
      if (cur_process_id >= processes_num) {
        cur_process_id = 1;
      }
    }
    params[0] = -1;
    for (int pid = 1; pid < processes_num; ++pid) {
      MPI_Send(params, 2, MPI_INT, pid, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    int chunks_num = cur_chunk_id;
    std::cout << "Merging " << chunks_num << " chunks." << std::endl;
    std::vector<std::ifstream> sorted_inputs;
    std::vector<int> sorted_inputs_size;
    std::ofstream out("sorted-array.txt");
    out << n << std::endl;
    std::set<std::pair<int, int>> min_elements;
    for (int i = 0; i < chunks_num; ++i) {
      sorted_inputs.push_back(std::ifstream("data/chunk-" + std::to_string(i)));
      int tmp;
      sorted_inputs[sorted_inputs.size() - 1] >> tmp;
      sorted_inputs_size.push_back(tmp);
      sorted_inputs[sorted_inputs.size() - 1] >> tmp;
      min_elements.insert(std::make_pair(tmp, i));
    }
    for (int i = 0; i < n; ++i) {
      auto min = *min_elements.begin();
      min_elements.erase(min_elements.begin());
      out << min.first << "\n";
      --sorted_inputs_size[min.second];
      if (sorted_inputs_size[min.second] > 0) {
        sorted_inputs[min.second] >> min.first;
        min_elements.insert(min);
      }
    }
    if (IsMasterProcess(rank)) {
      timer.stop();
    }
  } else {
    MPI_Status status;
    int params[2];
    std::vector<int> data(kChunkSize);
    Timer timer;
    timer.start();
    MPI_Recv(params, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    while (params[0] != -1) {
      int cur_size = params[0];
      MPI_Recv(&data[0], cur_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      std::sort(data.begin(), data.begin() + cur_size);
      std::ofstream out("data/chunk-" + std::to_string(params[1]));
      std::cout << "Process #" << rank << " sorted " << cur_size
                << " elements.";
      out << cur_size << std::endl;
      for (int i = 0; i < cur_size; ++i) {
        out << data[i] << " ";
      }
      out << std::endl;
      timer.stop();
      timer.start();
      MPI_Recv(params, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
  }

  MPI_Finalize();
}
