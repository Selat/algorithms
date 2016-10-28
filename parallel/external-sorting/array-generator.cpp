#include <fstream>
#include <iostream>
#include <random>
#include <chrono>
#include <climits>

int main() {
  size_t n;
  int cnum;
  std::cout << "Please enter array size: ";
  std::cin >> n;
  std::default_random_engine generator(
      std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<int> distribution(INT_MIN, INT_MAX);
  std::ofstream out("array.txt");
  out << n << std::endl;
  for (int i = 0; i < n; ++i) {
    out << distribution(generator) << " ";
  }
  out << std::endl;
}
