#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>
#include <cassert>

constexpr double EPS = 1E-6;

struct Point2D {
  double x, y;
};

std::istream& operator>>(std::istream& in, Point2D& point) {
  return in >> point.x >> point.y;
}

std::ostream& operator<<(std::ostream& out, const Point2D& point) {
  return out << "(" << point.x << " " << point.y << ")";
}

double ManhattanDistance(const Point2D& point1, const Point2D& point2) {
  return fabs(point1.x - point2.x) + fabs(point1.y - point2.y);
}

constexpr int kMaxSize = 5;

std::vector<Point2D> all_points;
std::vector<int> all_neighbours;

template <typename Iterator>
void Merge(Iterator begin1, Iterator end1, Iterator begin2, Iterator end2,
           Iterator out) {
  auto it1 = begin1;
  auto it2 = begin2;
  while (it1 != end1 && it2 != end2) {
    if (all_points[*it1].y < all_points[*it2].y) {
      *(out++) = *(it1++);
    } else {
      *(out++) = *(it2++);
    }
  }
  while (it1 != end1) {
    *(out++) = *(it1++);
  }
  while (it2 != end2) {
    *(out++) = *(it2++);
  }
}

void UpdateNeighbour(int id, int new_neighbour) {
  if (all_points[id].x > all_points[new_neighbour].x
      || all_points[id].y > all_points[new_neighbour].y) return;
  if (new_neighbour >= 0) {
    if (all_neighbours[id] >= 0) {
      double cur_dist = ManhattanDistance(all_points[id], all_points[all_neighbours[id]]);
      double new_dist = ManhattanDistance(all_points[id], all_points[new_neighbour]);
      if (new_dist < cur_dist) {
        all_neighbours[id] = new_neighbour;
      }
    } else {
      all_neighbours[id] = new_neighbour;
    }
  }
}

void BruteNENeighbours(const std::vector<Point2D>& points) {
  for (int i = 0; i < points.size(); ++i) {
    double min_dist = std::numeric_limits<double>::max();
    int closest_id = -1;
    for (int j = 0; j < points.size(); ++j) {
      if (i == j
          || points[j].x < points[i].x
          || points[j].y < points[i].y) continue;
      double dist = ManhattanDistance(points[i], points[j]);
      if (dist < min_dist) {
        min_dist = dist;
        closest_id = j;
      }
    }
    if (closest_id >= 0 && all_neighbours[i] >= 0) {
      double dist = ManhattanDistance(points[i], points[all_neighbours[i]]);
      if (min_dist < dist) {
        all_neighbours[i] = closest_id;
      }
    } else if (closest_id >= 0) {
      all_neighbours[i] = closest_id;
    }
  }
}

void FindNENeighbours(std::vector<int>& points, int l, int r) {
  if (r - l < kMaxSize) {
    for (int i = l; i < r; ++i) {
      double min_dist = std::numeric_limits<double>::max();
      int closest_id = -1;
      for (int j = l; j < r; ++j) {
        if (points[i] == points[j]
            || all_points[points[j]].x < all_points[points[i]].x
            || all_points[points[j]].y < all_points[points[i]].y) continue;
        double dist = ManhattanDistance(all_points[points[i]], all_points[points[j]]);
        if (dist < min_dist) {
          min_dist = dist;
          closest_id = points[j];
        }
      }
      UpdateNeighbour(points[i], closest_id);
    }
    std::sort(points.begin() + l, points.begin() + r, [](int p1, int p2) {
      return all_points[p1].y < all_points[p2].y;
    });
  } else {
    FindNENeighbours(points, l, l + (r - l) / 2);
    FindNENeighbours(points, l + (r - l) / 2, r);
    std::vector<int> left_points(points.begin() + l, points.begin() + l + (r - l) / 2);
    std::vector<int> right_points(points.begin() + l + (r - l) / 2, points.begin() + r);
    int left = left_points.size() - 1;
    int right = right_points.size() - 1;
    int min_id = right_points.size() - 1;
    double min_dist = -1;
    while (left >= 0 && all_points[right_points[min_id]].y < all_points[left_points[left]].y) --left;
    if (left >= 0) {
      min_dist = ManhattanDistance(all_points[left_points[left]],
        all_points[right_points[min_id]]);
    }
    while (right > 0 && left >= 0) {
      --right;
      while (left >= 0 && all_points[right_points[right]].y < all_points[left_points[left]].y) {
        UpdateNeighbour(left_points[left], right_points[min_id]);
        --left;
        if (left >= 0) {
          min_dist = ManhattanDistance(all_points[left_points[left]],
            all_points[right_points[min_id]]);
        }
      }
      if (left >= 0) {
        double cur_dist = ManhattanDistance(all_points[left_points[left]],
          all_points[right_points[right]]);
        if (cur_dist < min_dist) {
          min_id = right;
          min_dist = cur_dist;
        }
      }
    }
    while (left >= 0) {
      UpdateNeighbour(left_points[left], right_points[min_id]);
      --left;
    }

    Merge(left_points.begin(), left_points.end(),
          right_points.begin(), right_points.end(),
          points.begin() + l);
  }
}

void CheckNENeighbours() {
  all_neighbours.assign(n, -1);
  BruteNENeighbours(all_points);
  auto tmp = all_neighbours;
  all_neighbours.assign(all_points.size(), -1);

  std::vector<int> points(all_points.size());
  for (int i = 0; i < points.size(); ++i) {
    points[i] = i;
  }
  std::sort(points.begin(), points.end(), [](int p1, int p2) {
    return all_points[p1].x < all_points[p2].x;
  });
  FindNENeighbours(points, 0, points.size());
  for (int i = 0; i < all_points.size(); ++i) {
    if (all_neighbours[i] < 0) {
      assert(tmp[i] < 0);
      continue;
    }
    if (tmp[i] < 0) {
      assert(all_neighbours[i] < 0);
      continue;
    }
    double d1 = ManhattanDistance(all_points[i], all_points[all_neighbours[i]]);
    double d2 = ManhattanDistance(all_points[i], all_points[tmp[i]]);
    assert(fabs(d1 - d2) < EPS);
  }
  std::cout << "All NE Neighbours tests passed :)" << std::endl;
}

int main(int argc, char** argv) {
  assert(argc > 1);
  int n;
  std::ifstream in(argv[1]);
  std::ofstream out("output.txt");
  in >> n;
  std::cout << n << std::endl;
  all_points.resize(n);
  for (int i = 0; i < n; ++i) {
    in >> all_points[i];
  }
  // out << n << endl;
  // for (int i = 0; i < n; ++i) {
  //   double min_dist = std::numeric_limits<double>::max();
  //   int closest_id = -1;
  //   for (int j = 0; j < n; ++j) {
  //     if (i == j) continue;
  //     double dist = ManhattanDistance(all_points[i], all_points[j]);
  //     if (dist < min_dist) {
  //       min_dist = dist;
  //       closest_id = j;
  //     }
  //   }
  //   out << closest_id << endl;
  // }
  return 0;
}
