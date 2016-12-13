# Source: https://ru.wikipedia.org/wiki/%D0%9C%D0%B5%D1%82%D0%BE%D0%B4_%D1%81%D1%82%D1%80%D0%B5%D0%BB%D1%8C%D0%B1%D1%8B

import math
import pylab
import numpy as np
import matplotlib.pyplot as plt

a, b = 1.0, 2.0
A, B = 0.0, 1.0
n = 4
h = (b - a) / n
D0, D1 = A + h, h
s = 15

y = [[A, D0], [0, D1]]

def p(x):
    return 1.0

def q(x):
    return 1.0 / x

def f(x):
    return s * x * x


def get_c1():
    global n
    return (B - y[0][n]) / y[1][n]


def get_solv_y_i(i): return y[0][i] + get_c1() * y[1][i]

x = np.arange(a, b + 0.0001, h)


def div(a, b):
    return a / b

def realf(x):
    return (x ** 2 - 1) * (3 * s * (x ** 2 - 4) + 8) / 24.0

for i in range(1, n):
    y[0].append(
        div(
            (h ** 2 * f(x[i]) - (1.0 - (h / 2) * p(x[i])) * y[0][i - 1] - (h ** 2 * q(x[i]) - 2) * y[0][i]),
            1 + h / 2 * p(x[i])
        )
    )
    y[1].append(
        div(
            -(1 - h / 2 * p(x[i])) * y[1][i - 1] - (h ** 2 * q(x[i]) - 2) * y[1][i],
            1 + h / 2 * p(x[i])
        )
    )
yy = np.array([get_solv_y_i(i) for i in range(n + 1)])
print('Error: {}'.format(np.max(np.abs(realf(x) - yy))))

plt.plot(x, realf(x), label="exact solution")
plt.plot(x, yy, label="approximate solution")
plt.legend(loc=2)
plt.show()
