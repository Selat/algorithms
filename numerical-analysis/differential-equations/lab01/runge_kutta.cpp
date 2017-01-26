#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>

using namespace std;

typedef float datatype;

constexpr datatype kAlpha = 0.8;

datatype F(datatype x, datatype y) {
  return -3.1 * pow(x, 2.1) * y * sin(pow(x, 3.1));
}

datatype RealF(datatype x) {
  return exp(cos(pow(x, 3.1)));
}

vector<datatype> RungeKutta(datatype h, datatype a, datatype b, datatype eta) {
  datatype x = a;
  vector<datatype> ay;
  ay.push_back(eta);
  while (x <= b) {
    datatype y = ay.back();
    datatype k1 = h * F(x, y);
    datatype k2 = h * F(x + h / 2.0, y + k1 / 2.0);
    datatype k3 = h * F(x + h / 2.0, y + k2 / 2.0);
    datatype k4 = h * F(x + h, y + k3);
    datatype newy = y + (k1 + 2.0 * k2 + 2.0 * k3 + k4) / 6.0;
    ay.push_back(newy);
    x += h;
  }
  return ay;
}

datatype CalculateRungeKutta(datatype x, datatype prevy, datatype h) {
  datatype k1 = h * F(x, prevy);
  datatype k2 = h * F(x + h / 2.0, prevy + k1 / 2.0);
  datatype k3 = h * F(x + h / 2.0, prevy + k2 / 2.0);
  datatype k4 = h * F(x + h, prevy + k3);
  datatype newy = prevy + (k1 + 2.0 * k2 + 2.0 * k3 + k4) / 6.0;
  return newy;
}

datatype GetAbsError(datatype x, datatype y, datatype h) {
  datatype res1 = CalculateRungeKutta(x, y, h);
  datatype tmp = CalculateRungeKutta(x, y, h * 0.5);
  datatype res2 = CalculateRungeKutta(x + h * 0.5, tmp, h * 0.5);
  return abs(res1 - res2);
}

pair<vector<datatype>, vector<datatype>> AdaptiveRungeKutta(
  datatype initialh, datatype a, datatype b, datatype eta,
  datatype eps, int& niterations) {
  niterations = 0;
  datatype x = a;
  vector<datatype> ay;
  vector<datatype> ax;
  ay.push_back(eta);
  ax.push_back(0.0);
  datatype h = initialh;
  while (x < b) {
    datatype y = ay.back();
    int i = 0;
    datatype error;
    for (i = 0; i < 5; ++i) {
      error = GetAbsError(x, y, h);
      ++niterations;
      if (error < eps) break;
      // 2^k - 1 = 15
      datatype delta = pow(15.0 * eps / error, 0.2);
      h = h * kAlpha * delta * 0.5;
    }
    assert(error <= eps);

    if (x + h > b) {
      h = b - x;
    }
    ay.push_back(CalculateRungeKutta(x, y, h));
    x += h;
    ax.push_back(x);
  }
  return make_pair(ax, ay);
}

datatype GetRungeKuttaAbsoluteError(datatype eps, int& niterations) {
  auto v = AdaptiveRungeKutta(0.2, 0.0, 1.3, exp(1.0), eps, niterations);
  datatype error = 0.0;
  for (int i = 0; i < v.first.size(); ++i) {
    error = max(error, abs(v.second[i] - RealF(v.first[i])));
  }
  return error;
}

datatype GetRungeKuttaRelativeError(datatype h) {
  auto v = RungeKutta(h, 0.0, 1.3, exp(1.0));
  datatype x = 0.0;
  datatype error = 0.0;
  for (auto y : v) {
    error = max(error, abs((y - RealF(x)) / RealF(x)));
    x += h;
  }
  return error;
}

class FDiffer {
 public:
  FDiffer(const vector<datatype>& ty, datatype ta, datatype th)
      : y(ty), a(ta), h(th) {}
  datatype Get(int i) {
    return F(a + h * i, y[i]);
  }
 private:
  vector<datatype> y;
  datatype a;
  datatype h;
};

vector<datatype> Adams(datatype h, datatype a, datatype b,
                       datatype eta, int& niterations) {
  auto ty = RungeKutta(h, a, b, eta);
  vector<datatype> y(ty);
  int n = y.size();
  FDiffer d(ty, a, h);
  for (int i = 3; i + 1 < n; ++i) {
    y[i + 1] = y[i] + (55.0 * d.Get(i) - 59.0 * d.Get(i - 1) +
     37.0 * d.Get(i - 2) - 9.0 * d.Get(i - 3)) * h / 24.0;
    ++niterations;
  }
  return y;
}

datatype GetAdamsError(datatype h) {
  int niterations;
  auto v = Adams(h, 0.0, 1.3, exp(1.0), niterations);
  datatype x = 0.0;
  datatype error = 0.0;
  for (auto y : v) {
    error = max(error, abs(y - RealF(x)));
    x += h;
  }
  return error;
}

void PrintRungeKutta() {
  cout << "Runge Kutta method: " << endl;
  datatype h = 0.2;
  datatype eps = 0.001;
  int niterations;
  auto res = AdaptiveRungeKutta(h, 0.0, 1.3, exp(1.0), eps, niterations);
  datatype max_error = 0.0;
  int max_error_id = -1;
  cout << "Size: " << res.first.size() << " Iterations: " << niterations << endl;
  for (int i = 0; i < res.first.size(); ++i) {
    datatype cur_error = abs(RealF(res.first[i]) - res.second[i]);
    if (cur_error > max_error) {
      max_error = cur_error;
      max_error_id = i;
    }
  }
  cout << max_error << endl;
  //cout << "Accepted h: " << h << " (" << niterations << " iterations, " << GetRelError(h) << " error)" << endl;
  auto v1 = RungeKutta(h, 0.0, 1.3, exp(1.0));
  auto v2 = RungeKutta(h / 2.0, 0.0, 1.3, exp(1.0));
  for (int i = 0; i < v1.size(); ++i) {
    cout << h * i << " & ";
  }
  cout << endl;
  for (auto y : v1) {
    cout << y << " & ";
  }
  cout << endl;
  for (int i = 0; i < v2.size(); ++i) {
    cout << h * i / 2 << " & ";
  }
  cout << endl;
  for (auto y : v2) {
    cout << y << " & ";
  }
  cout << endl;
  cout << "Absolute error: " << endl;
  datatype curx = 0.0;
  for (int i = 0; i < v1.size(); ++i) {
    cout << " & " << curx;
    curx += h;
  }
  cout << endl;
  curx = 0.0;
  for (auto y : v1) {
    cout << " & " << abs(y - RealF(curx));
    curx += h;
  }
  cout << endl;
  cout << "Relative error: " << endl;
  curx = 0.0;
  for (int i = 0; i < v1.size(); ++i) {
    cout << " & " << curx;
    curx += h;
  }
  cout << endl;
  curx = 0.0;
  for (auto y : v1) {
    cout << " & " << abs((y - RealF(curx)) / RealF(curx));
    curx += h;
  }
  cout << endl;
  cout << "Major error summand: " << endl;
  curx = 0.0;
  for (int i = 0; i < v1.size() && i * 2 < v2.size(); ++i) {
    cout << " & " << curx;
    curx += h;
  }
  cout << endl;
  for (int i = 0; i < v1.size() && i * 2 < v2.size(); ++i) {
    cout << " & " << abs(v1[i] - v2[i * 2]) / 15.0;
    curx += h;
  }
  cout << endl << endl;

  cout << "Errors graph: ";
  cout << endl;
  constexpr int total_q = 5;
  vector<int> niters;
  for (int i = 1; i <= total_q; ++i) {
    datatype eps = pow(5, datatype(i) / total_q) * 0.00001;
    cout << eps << "-";
    int niterations;
    cout << GetRungeKuttaAbsoluteError(eps, niterations) << ", ";
    niters.push_back(niterations);
  }
  cout << endl;
  for (auto t : niters) {
    cout << t << ", ";
  }
  cout << endl;
}

void PrintAdams() {
  cout << "Adams method: " << endl;
  int niterations;
  auto data = Adams(0.005, 0.0, 1.3, exp(1.0), niterations);
  cout << "Adams iterations: " << niterations << endl;

  for (int i = 0; i < 20; ++i) {
    cout << 0.005 * i << " & ";
  }
  cout << endl;
  for (int i = 0; i < 20; ++i) {
    cout << data[i] << " & ";
  }
  cout << endl;

  for (int i = data.size() - 20; i < data.size(); ++i) {
    cout << 0.005 * i << " & ";
  }
  cout << endl;
  for (int i = 0; i < 20; ++i) {
    cout << data[i] << " & ";
  }
  cout << endl;
  for (int i = data.size() - 20; i < data.size(); ++i) {
    cout << data[i] << " & ";
  }
  cout << endl;

  constexpr int total_q = 5;
  for (int i = 0; i < total_q; ++i) {
    datatype h = pow(5, datatype(i) / total_q) * 0.0001;
    cout << 1.3 / h * 4 << ", ";
  }
  cout << endl;
  for (int i = 0; i < total_q; ++i) {
    datatype h = pow(5, datatype(i) / total_q) * 0.0001;
    cout << GetAdamsError(h) << ", ";
  }
  cout << endl;
}

int main() {
  PrintRungeKutta();
  PrintAdams();
  return 0;
}
