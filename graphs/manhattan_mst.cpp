#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>
#include <cassert>

using namespace std;

constexpr double EPS = 1E-6;

struct Point2D {
  double x, y;
};

istream& operator>>(istream& in, Point2D& point) {
  return in >> point.x >> point.y;
}

ostream& operator<<(ostream& out, const Point2D& point) {
  return out << "(" << point.x << " " << point.y << ")";
}

double ManhattanDistance(const Point2D& point1, const Point2D& point2) {
  return fabs(point1.x - point2.x) + fabs(point1.y - point2.y);
}

constexpr int kMaxSize = 5;

std::vector<Point2D> all_points;
std::vector<int> all_neighbours;

template <typename T>
void Merge(const std::vector<T>& a, const std::vector<T>& b, std::vector<T>& out) {
  auto it1 = a.begin();
  auto it2 = b.begin();
  auto out_it = out.begin();
  while (it1 != a.end() && it2 != b.end()) {
    if (all_points[*it1].y < all_points[*it2].y) {
      *(out_it++) = *(it1++);
    } else {
      *(out_it++) = *(it2++);
    }
  }
  while (it1 != a.end()) {
    *(out_it++) = *(it1++);
  }
  while (it2 != b.end()) {
    *(out_it++) = *(it2++);
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

void FindNENeighbours(std::vector<int>& points) {
  if (points.size() < kMaxSize) {
    // std::cout << "Brute<<<<<<<" << std::endl;
    for (int i = 0; i < points.size(); ++i) {
      double min_dist = std::numeric_limits<double>::max();
      int closest_id = -1;
      for (int j = 0; j < points.size(); ++j) {
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
    std::sort(points.begin(), points.end(), [](int p1, int p2) {
      return all_points[p1].y < all_points[p2].y;
    });
    // std::cout << ">>>>>>>Brute" << std::endl;
  } else {
    std::vector<int> sorted_points = points;
    std::sort(sorted_points.begin(), sorted_points.end(),
              [](int p1, int p2) {
      return all_points[p1].x < all_points[p2].x;
    });
    double med_x = all_points[sorted_points[sorted_points.size() / 2]].x;
    std::vector<int> left_points;
    std::vector<int> right_points;
    for (auto p : sorted_points) {
      if (all_points[p].x <= med_x) {
        left_points.push_back(p);
      } else {
        right_points.push_back(p);
      }
    }
    FindNENeighbours(left_points);
    FindNENeighbours(right_points);
    // std::cout << "Split: " << med_x << " " << left_points.size() << " " << right_points.size() << std::endl;
    // for (auto l : left_points) {
    //   std::cout << l << " " << all_points[l] << std::endl;
    // }
    // std::cout << "----------" << std::endl;
    // for (auto l : right_points) {
    //   std::cout << l << " " << all_points[l] << std::endl;
    // }
    // std::cout << "==========" << std::endl;

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
      // std::cout << left << " " << right << std::endl;
      --right;
      while (left >= 0 && all_points[right_points[right]].y < all_points[left_points[left]].y) {
        // std::cout << "ok for " << left_points[left] << " " << right_points[right] << std::endl;
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
        // std::cout << "Dists: " << cur_dist << " " << min_dist << std::endl;
        // std::cout << left_points[left] << " " << right_points[right] << " " << right_points[min_id] << std::endl;
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

    Merge(left_points, right_points, points);
  }
}

int main(int argc, char** argv) {
  assert(argc > 1);
  int n;
  ifstream in(argv[1]);
  ofstream out("output.txt");
  in >> n;
  std::cout << n << std::endl;
  all_points.resize(n);
  all_neighbours.assign(n, -1);
  for (int i = 0; i < n; ++i) {
    in >> all_points[i];
  }
  BruteNENeighbours(all_points);
  auto tmp = all_neighbours;
  all_neighbours.assign(n, -1);

  std::vector<int> points(n);
  for (int i = 0; i < n; ++i) {
    points[i] = i;
  }
  FindNENeighbours(points);
  for (int i = 0; i < all_points.size(); ++i) {
    // std::cout << i << " " << all_neighbours[i] << " " << tmp[i] << std::endl;
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
    // assert(d1 >= d2);
    // std::cout << d1 << " " << d2 << std::endl;
    assert(fabs(d1 - d2) < EPS);
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
