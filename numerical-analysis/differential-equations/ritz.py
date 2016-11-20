import sympy
import numpy as np

class Limit:
    def __init__(self, alpha0, alpha1, a):
        self.alpha0 = alpha0
        self.alpha1 = alpha1
        self.a = a

# Solves differential equation of the form
# L(u) = u''(x) + p(x)u'(x) + q(x)u(x) = f(x)
# Idea:
# u(x) \approx \sum_{i = 0}^{\infty}\dfrac{u^{(i)}(\xi)}{i!}(x - \xi)^i
def solve(p, q, f, phi, a, b):
    res = sympy.Integer(0)
    x = sympy.symbols('x', real=True)
    u = sympy.symbols('u', cls=sympy.Function)
    n = len(phi) - 1
    aa = np.zeros((n, n))
    for i in range(0, n):
        cure = (phi[i + 1].diff(x).diff(x) + p * phi[i + 1].diff(x) + q * phi[i + 1])
        cure = sympy.expand(cure)
        for j in range(0, n):
            nexte = (phi[j + 1].diff(x).diff(x) + p * phi[j + 1].diff(x) + q * phi[j + 1])
            nexte = sympy.expand(nexte)
            print(i, j, cure * nexte)
            aa[i, j] = sympy.integrate(sympy.expand(cure * nexte), (x, a, b))
    bb = np.zeros((n))
    for i in range(0, n):
        cure = (phi[i + 1].diff(x).diff(x) + p * phi[i + 1].diff(x) + q * phi[i + 1])
        cure = sympy.expand(cure)
        cure *= (6 * x - x**4 + x**2)
        print(cure)
        bb[i] = sympy.integrate(cure, (x, a, b))
        print(x, a, b)
    return aa, bb
