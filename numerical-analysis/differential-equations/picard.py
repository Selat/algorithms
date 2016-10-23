import sympy

# Solves differential equation of the form
# u'(x) = f(x, u(x)), u(\xi) = \eta
def solve(f, xi, eta, n_iterations=1):
    cury = eta
    tau = sympy.symbols('tau', real=True)
    x = sympy.symbols('x', real=True)
    u = sympy.symbols('u')
    for i in range(0, n_iterations):
        cury = eta + sympy.integrate(f.subs([(u, cury), (x, tau)]), (tau, xi, x))
    return cury
