#include <iostream>
#include <thread>

class CopyAware {
public:
  CopyAware() = default;
  CopyAware(const CopyAware& copy_aware) {
    std::cout << "copy constructor is called" << std::endl;
  }
};

int main() {
  CopyAware copy_aware;
  std::cout << "Preparing to start a new thrad" << std::endl;
  int value = 10;
  std::thread mythread([](const CopyAware& copy_aware, int& value){
    std::cout << "New thread is running" << std::endl;
    for (volatile int i = 0; i < 1000 * 1000 * 1000; ++i);
    value = 100;
  }, copy_aware, std::ref(value));
  mythread.join();
  std::cout << "Updated value: " << value << std::endl;
  return 0;
}
