import numpy as np

# Solves differential equation of the form
# u'(x) = f(x, u(x)), x \in [a, b], u(a) = \eta
# Explicit euler's method:
# y_{n + 1} = y_n + h * f(x_n, y_n), y_n \approx u(x_n)
def explicit_solve(f, a, b, eta, n=2):
    points = np.linspace(a, b, num=n)
    h = (b - a) / (n - 1)
    y = np.zeros((n), dtype='float64')
    y[0] = eta
    for i, x in enumerate(points[:-1]):
        y[i + 1] = y[i] +  h * f(x, y[i])
    return y
