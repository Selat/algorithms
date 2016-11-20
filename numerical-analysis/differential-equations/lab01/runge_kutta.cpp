#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

typedef float datatype;

datatype F(datatype x, datatype y) {
  return -3.1 * pow(x, 2.1) * y * sin(pow(x, 3.1));
}

datatype RealF(datatype x) {
  return exp(cos(pow(x, 3.1)));
}

std::vector<datatype> RungeKutta(datatype h, datatype a, datatype b, datatype eta) {
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

datatype GetRelError(datatype h) {
  auto v1 = RungeKutta(h, 0.0, 1.3, exp(1.0));
  auto v2 = RungeKutta(h / 2.0, 0.0, 1.3, exp(1.0));
  datatype error = 0.0;
  for (int i = 0; i < v1.size() && i * 2 < v2.size(); ++i) {
    error = max(error, abs(v1[i] - v2[i * 2]));
  }
  return error;
}

datatype GetRungeKuttaAbsoluteError(datatype h) {
  auto v = RungeKutta(h, 0.0, 1.3, exp(1.0));
  datatype x = 0.0;
  datatype error = 0.0;
  for (auto y : v) {
    error = max(error, abs(y - RealF(x)));
    x += h;
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

vector<datatype> Adams(datatype h, datatype a, datatype b, datatype eta) {
  auto ty = RungeKutta(h, a, b, eta);
  vector<datatype> y(ty);
  int n = y.size();
  FDiffer d(ty, a, h);
  for (int i = 3; i + 1 < n; ++i) {
    //y[i + 1] = y[i] + (55.0 * (ty[i + 1] - ty[i]) - 59.0 * (ty[i] - ty[i - 1]) + 37.0 * (ty[i - 1] - ty[i - 2]) - 9.0 * (ty[i - 2] - ty[i - 3])) / 24.0;
    y[i + 1] = y[i] + (55.0 * d.Get(i) - 59.0 * d.Get(i - 1) + 37.0 * d.Get(i - 2) - 9.0 * d.Get(i - 3)) * h / 24.0;
  }
  return y;
}

datatype GetAdamsError(datatype h) {
  auto v = Adams(h, 0.0, 1.3, exp(1.0));
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
  datatype error = 1.0;
  const datatype k = 4.0;
  int niterations = 0;
  for (int i = 0; i < 10; ++i) {
    error = GetRelError(h);
    h = h / 2 * pow((pow(2.0, k) - 1.0) * eps / error, 1 / k);
    ++niterations;
    if (GetRelError(h) < eps) break;
  }
  cout << "Accepted h: " << h << " (" << niterations << " iterations, " << GetRelError(h) << " error)" << endl;
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

  constexpr int total_q = 5;
  for (int i = 1; i <= total_q; ++i) {
    datatype h = pow(5, datatype(i) / total_q) * 0.0001;
    cout << (1.3 / h * 4) << ", ";
  }
  cout << endl;
  for (int i = 1; i <= total_q; ++i) {
    datatype h = pow(5, datatype(i) / total_q) * 0.0001;
    cout << GetRungeKuttaAbsoluteError(h) << ", ";
  }
  cout << endl;
}

void PrintAdams() {
  cout << "Adams method: " << endl;
  auto data = Adams(0.005, 0.0, 1.3, exp(1.0));
  cout << "Global error: " << GetAdamsError(0.005) << endl;

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
