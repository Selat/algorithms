#include <iostream>
#include <fstream>
#include <random>

using namespace std;

int main() {
  int n;
  double min_val, max_val;
  cout << "Enter number of points: ";
  cin >> n;
  cout << "Min: ";
  cin >> min_val;
  cout << "Max: ";
  cin >> max_val;

  default_random_engine generator;
  uniform_real_distribution<double> distribution(min_val, max_val);

  ofstream out("input.txt");
  out << n << endl;
  for (int i = 0; i < n; ++i) {
    out << distribution(generator) << " " << distribution(generator) << endl;
  }
  return 0;
}
