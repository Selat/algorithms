import sympy

# Solves differential equation of the form
# u'(x) = f(x, u(x)), u(\xi) = \eta
# Iteration:
# y_n = \eta + \int_{\xi}^xf(\tau, y_{n - 1}(\tau))d\tau
def solve(f, xi, eta, n_iterations=1):
    cury = eta
    tau = sympy.symbols('tau', real=True)
    x = sympy.symbols('x', real=True)
    u = sympy.symbols('u', cls=sympy.Function)
    for i in range(0, n_iterations):
        cury = eta + sympy.integrate(f.subs([(u(x), cury), (x, tau)]), (tau, xi, x))
    return cury
