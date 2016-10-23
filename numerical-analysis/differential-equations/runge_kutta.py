import numpy as np

def solve(f, a, b, eta, alpha, beta, p, n=2):
    q = len(p)
    k = np.zeros((q), dtype='float64')
    y = np.zeros((n), dtype='float64')
    points = np.linspace(a, b, num=n)
    h = (b - a) / (n - 1)
    for i, x in enumerate(points[:-1]):
        k[0] = h * f(x, y[i])
        tsum = p[0] * k[0]
        for j in range(1, q):
            k[j] = h * f(x + alpha[j - 1] * h,
                         y[i] + np.dot(beta[j - 1,:j], k[:j]))
            tsum += p[j] * k[j]
        y[i + 1] = y[i] + tsum
    return y

# Solves differential equation using 2nd order of accuracy
def solve_2(f, a, b, eta, n=2):
    alpha = np.asarray([0.5], dtype='float64')
    beta = np.asarray([[0.5]], dtype='float64')
    p = np.asarray([0.0, 1.0], dtype='float64')
    return solve(f, a, b, eta, alpha, beta, p, n)


# Solves differential equation using 3rd order of accuracy
def solve_3(f, a, b, eta, n=2):
    alpha = np.asarray([0.5, 1.0], dtype='float64')
    beta = np.asarray([[0.5, 0.0], [-1.0, 2.0]], dtype='float64')
    p = np.asarray([1.0 / 6.0, 4.0 / 6.0, 1.0 / 6.0], dtype='float64')
    return solve(f, a, b, eta, alpha, beta, p, n)

# Solves differential equation using 4th order of accuracy
def solve_4(f, a, b, eta, n=2):
    alpha = np.asarray([0.5, 0.5, 1.0], dtype='float64')
    beta = np.asarray([[0.5, 0.0, 0.0], [0.0, 0.5, 0.0], [0.0, 0.0, 1.0]],
                      dtype='float64')
    p = np.asarray([1.0 / 6.0, 2.0 / 6.0, 2.0 / 6.0, 1.0 / 6.0],
                   dtype='float64')
    return solve(f, a, b, eta, alpha, beta, p, n)
