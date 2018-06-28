#pragma once

#include <cmath>

class PixelWalker {
public:
  PixelWalker(int start_x, int start_y, float angle);

  int getX() const {
    return cur_x_;
  }

  int getY() const {
    return cur_y_;
  }

  void next() {
    cur_x_ += dx_;
    cur_y_ += dy_;
  }
private:
  float cur_x_;
  float cur_y_;
  float dx_;
  float dy_;
};

PixelWalker::PixelWalker(int start_x, int start_y, float angle)
    : cur_x_(start_x), cur_y_(start_y) {
  dx_ = cos(angle);
  dy_ = sin(angle);
  float norm = fmax(fabs(dx_), fabs(dy_));
  dx_ /= norm;
  dy_ /= norm;
}
