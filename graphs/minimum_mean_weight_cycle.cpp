#include <iostream>
#include <vector>
#include <fstream>
#include <limits>
#include <cassert>
#include <set>

constexpr double INF = std::numeric_limits<double>::max();

// Assumes that graph G is strongly connected
double MinimumMeanWeightCycle(int startv,
    const std::vector<std::vector<std::pair<int, double>>>& g) {
  std::vector<std::vector<double>> d(g.size(),
      std::vector<double>(g.size() + 1, INF));
  std::vector<bool> visited(g.size(), false);
  d[startv][startv] = 0.0;
  visited[startv] = true;
  for (int k = 0; k <= g.size(); ++k) {
    for (int v = 0; v < g.size(); ++v) {
      for (int j = 0; j < g[v].size(); ++j) {
        int to = g[v][j].first;
        double w = g[v][j].second;
        if (d[v][k - 1] == INF) continue;
        visited[to] = true;
        d[to][k] = std::min(d[v][k - 1] + w, d[to][k]);
      }
    }
  }

  double ans = std::numeric_limits<double>::max();
  for (int v = 0; v < g.size(); ++v) {
    if (!visited[v]) continue;
    double cur_ans = std::numeric_limits<double>::min();
    for (int k = 0; k < g.size(); ++k) {
      double tmp = (d[v][g.size()] - d[v][k]) / (g.size() - k);
      if (tmp > cur_ans) cur_ans = tmp;
    }
    if (cur_ans < ans) ans = cur_ans;
  }
  return ans;
}

void dfs1(int v, std::vector<int>& order, std::vector<bool>& used,
          const std::vector<std::vector<std::pair<int, double>>>& g) {
  used[v] = true;
  for (int i = 0; i < g[v].size(); ++i) {
    if (!used[g[v][i].first]) {
      dfs1(g[v][i].first, order, used, g);
    }
  }
  order.push_back(v);
}

void dfs2(int v, std::vector<int>& component, std::vector<bool>& used,
          const std::vector<std::vector<std::pair<int, double>>>& g) {
  used[v] = true;
  component.push_back(v);
  for (int i = 0; i < g[v].size(); ++i) {
    if (!used[g[v][i].first]) {
      dfs2(g[v][i].first, component, used, g);
    }
  }
}

int main(int argc, char** argv) {
  assert(argc == 2);
  int n;
  std::ifstream in(argv[1]);
  in >> n;
  std::cout << n << std::endl;
  std::vector<std::vector<std::pair<int, double>>> g(n);
  for (int i = 0; i < n; ++i) {
    int neighbors_num;
    in >> neighbors_num;
    for (int j = 0; j < neighbors_num; ++j) {
      int nid;
      double w;
      in >> nid >> w;
      g[i].push_back(std::make_pair(nid, w));
    }
  }

  // Find strongly connected components
  std::vector<int> order;
  std::vector<bool> used;
  used.assign(n, false);
  for (int i = 0; i < n; ++i) {
    if (!used[i]) dfs1(i, order, used, g);
  }
  std::vector<int> component;
  used.assign(n, false);
  for (int i = 0; i < n; ++i) {
    if (!used[i]) {
      std::vector<int> component;
      dfs2(i, component, used, g);
      std::set<int> sc(component.begin(), component.end());
      std::vector<std::vector<std::pair<int, double>>> tg(n);
      for (auto i : sc) {
        for (auto n : g[i]) {
          if (sc.count(n.first)) {
            tg[i].push_back(n);
          }
        }
      }
      std::cout << MinimumMeanWeightCycle(*sc.begin(), tg) << std::endl;
    }
  }
  return 0;
}
