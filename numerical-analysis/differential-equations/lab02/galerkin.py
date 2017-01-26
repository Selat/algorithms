import matplotlib.pyplot as plt
import numpy as np
import sympy

s = 15
x = sympy.symbols('x')

def realf(x):
    return (x ** 2 - 1) * (3 * s * (x ** 2 - 4) + 8) / 24.0

def L(y):
    return sympy.diff(y, x, x) - sympy.diff(y, x) / x

n = 2
a = 1.0
b = 2.0
f = s * (x ** 2)
u = (x ** 2 - 1) * (3 * s * (x ** 2 - 4) + 8) / 24.0
phi = [x - 1, (x - 1) * (2 - x), (2 - x) * (x - 1) ** 2]
a11 = sympy.N(sympy.integrate(L(phi[1]) * phi[1], (x, a, b)))
a12 = sympy.N(sympy.integrate(L(phi[1]) * phi[2], (x, a, b)))
a21 = sympy.N(sympy.integrate(L(phi[2]) * phi[1], (x, a, b)))
a22 = sympy.N(sympy.integrate(L(phi[2]) * phi[2], (x, a, b)))
diff = f - L(phi[0])
d1 = sympy.N(sympy.integrate(phi[1] * diff, (x, a, b)))
d2 = sympy.N(sympy.integrate(phi[2] * diff, (x, a, b)))
A = sympy.Matrix([[a11, a12], [a21, a22]])
D = sympy.Matrix([[d1], [d2]])
sol = A.LUsolve(D)
print('a = {0}'.format(sol))
y = phi[0] + sol[0] * phi[1] + sol[1] * phi[2]
max_error = 0
for curx in np.linspace(a + (b - a) / 20.0, b, num=20):
    max_error = max(abs((realf(curx) - y.subs(x, curx)) / realf(curx)), max_error)

print('phi_0 = {0}'.format(phi[0]))
print('phi_1 = {0}'.format(phi[1]))
print('phi_2 = {0}'.format(phi[2]))
print('A = {0}'.format(A))
print('D = {0}'.format(D))
print('max error = {0}'.format(max_error))

xx = np.arange(a, b, 0.02)

plt.plot(xx, realf(xx), label="exact")
plt.plot(xx, xx - 1 + sol[0] * (xx - 1) * (2 - xx) + sol[1] * (2 - xx) * (xx - 1) ** 2, label="approximation")
plt.legend(loc=2)

plt.show()
