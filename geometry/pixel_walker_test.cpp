#include "pixel_walker.h"

#include <cassert>
#include <iostream>

void RunTestStraightX() {
  PixelWalker walker(0, 0, 0.0f);
  for (int i = 0; i < 100; ++i) {
    walker.next();
    assert(walker.getX() == i + 1);
    assert(walker.getY() == 0);
  }
}

void RunTestStraightXInvert() {
  PixelWalker walker(0, 0, M_PI);
  for (int i = 0; i < 100; ++i) {
    walker.next();
    assert(walker.getX() == -i - 1);
    assert(walker.getY() == 0);
  }
}

void RunTestStraightY() {
  PixelWalker walker(0, 0, M_PI_2);
  for (int i = 0; i < 100; ++i) {
    walker.next();
    assert(walker.getX() == 0);
    assert(walker.getY() == i + 1);
  }
}

void RunTestStraightYInvert() {
  PixelWalker walker(0, 0, M_PI_2 * 3.0f);
  for (int i = 0; i < 100; ++i) {
    walker.next();
    assert(walker.getX() == 0);
    assert(walker.getY() == -i - 1);
  }
}

void RunTestDegrees45() {
  PixelWalker walker(0, 0, M_PI_4);
  for (int i = 0; i < 100; ++i) {
    walker.next();
    assert(walker.getX() == i + 1);
    assert(walker.getY() == i + 1);
  }
}

void RunTestDegrees30() {
  PixelWalker walker(0, 0, M_PI / 6.0f);
  walker.next();
  assert(walker.getX() == 1);
  assert(walker.getY() == 0);
  walker.next();
  assert(walker.getX() == 2);
  assert(walker.getY() == 1);
  walker.next();
  assert(walker.getX() == 3);
  assert(walker.getY() == 1);
  walker.next();
  assert(walker.getX() == 4);
  assert(walker.getY() == 2);
  walker.next();
  assert(walker.getX() == 5);
  assert(walker.getY() == 2);
  walker.next();
  assert(walker.getX() == 6);
  assert(walker.getY() == 3);
  walker.next();
  assert(walker.getX() == 7);
  assert(walker.getY() == 4);
  walker.next();
  assert(walker.getX() == 8);
  assert(walker.getY() == 4);
  walker.next();
  assert(walker.getX() == 9);
  assert(walker.getY() == 5);
}

int main() {
  RunTestStraightX();
  RunTestStraightY();
  RunTestStraightXInvert();
  RunTestStraightYInvert();
  RunTestDegrees30();
  RunTestDegrees45();
  std::cout << "All tests passed! :)" << std::endl;
  return 0;
}
