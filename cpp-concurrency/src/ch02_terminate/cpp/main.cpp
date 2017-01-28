#include <thread>
#include <iostream>

void test() {
  std::cout << "hello" << std::endl;
}

void createThread() {
  std::thread mythread(test);
  throw std::exception();
  mythread.join();
}

int main() {
  // If std::thread destructor is executed before .detach() or .join() it
  // calls std::terminate().
  try {
    createThread();
  } catch (std::exception& e) {

  }
  return 0;
}
