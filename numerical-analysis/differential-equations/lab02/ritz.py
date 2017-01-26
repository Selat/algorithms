import sympy
import imp
import scipy.linalg as sla
import matplotlib.pyplot as plt
import numpy as np


def realf(x):
    return (x ** 2 - 1) * (3 * s * (x ** 2 - 4) + 8) / 24.0

def solve(p, q, f, phi, a, b):
    res = sympy.Integer(0)
    x = sympy.symbols('x', real=True)
    u = sympy.symbols('u', cls=sympy.Function)
    n = len(phi) - 1
    aa = np.zeros((n, n))
    for i in range(0, n):
        for j in range(0, n):
            cure = p * phi[i + 1].diff(x) * phi[j + 1].diff(x) + q * phi[i + 1] * phi[j + 1]
            aa[i, j] = sympy.integrate(cure, (x, a, b))
    bb = np.zeros((n))
    for i in range(0, n):
        cure = p * phi[0].diff(x) * phi[i + 1].diff(x) + q * phi[0] * phi[i + 1] + f * phi[i + 1]
        bb[i] = sympy.integrate(cure, (x, a, b))
    return aa, bb

# My variant
s = 15

x = sympy.symbols('x', real=True)
p = 1
q = 1 / x
f = s * x * x
phi = [x - 1, (x - 1) * (2 - x), (x - 1) * (x - 1) * (2 - x)]
a = 1
b = 2
Am, Bm = solve(p, q, f, phi, a, b)
sol = sla.solve(Am, -Bm)
xx = np.linspace(a + (b - a) / 20.0, b, num=20)
approxf = lambda x : (xx - 1 + sol[0] * (xx - 1) * (2 - xx) + sol[1] * (2 - xx) * (xx - 1) ** 2)
print('Max error: {}'.format(np.max(np.abs(realf(xx) - approxf(xx)))))
print('A: {}'.format(Am))
print('B: {}'.format(-Bm))
print('Solution: {}'.format(sol))
xx = np.arange(a, b, 0.02)
plt.plot(xx, (xx ** 2 - 1) * (3 * s * (xx ** 2 - 4) + 8) / 24.0, label="exact")
plt.plot(xx, xx - 1 + sol[0] * (xx - 1) * (2 - xx) + sol[1] * (2 - xx) * (xx - 1) ** 2, label="approximation")
plt.legend(loc=2)
plt.show()
