import sympy
import imp
import ritz2
import scipy.linalg as sla
import matplotlib.pyplot as plt
import numpy as np

# Solves differential equation of the form
# (p(x)u'(x))' - q(x)u(x) = f(x), a <= x <= b
# u(a) = A, u(b) = B
# Idea:
# u_n(x) = phi_0(x) + \sum\limits_{k = 1}^n a_k\phi_k(x)
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
t1 = np.arange(a, b, 0.02)
plt.plot(t1, (t1 ** 2 - 1) * (3 * s * (t1 ** 2 - 4) + 8) / 24.0, label="exact solution")
plt.plot(t1, t1 - 1 + sol[0] * (t1 - 1) * (2 - t1) + sol[1] * (2 - t1) * (t1 - 1) ** 2, label="approximate solution")
plt.legend(loc=2)
plt.show()
