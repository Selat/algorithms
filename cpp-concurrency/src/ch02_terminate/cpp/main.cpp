#include <thread>
#include <iostream>

void slowFunction() {
  for (int i = 0; i < 1000 * 1000; ++i);
}

int main() {
  // If main is finished before mythread.join() or mythrad.detach() is called,
  // std::thread destructor calls std::terminate().
  std::thread mythread(slowFunction);
  return 0;
}
