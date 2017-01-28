#include <iostream>
#include <thread>

void test() {
  std::cout << "hello concurrency\n";
}

int main() {
  std::thread t(test);
  t.join();
  return 0;
}
