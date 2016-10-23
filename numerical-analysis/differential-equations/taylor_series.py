import sympy

# Solves differential equation of the form
# u'(x) = f(x, u(x)), u(\xi) = \eta
# Idea:
# u(x) \approx \sum_{i = 0}^{\infty}\dfrac{u^{(i)}(\xi)}{i!}(x - \xi)^i
def solve(f, xi, eta, n_iterations=1):
    res = sympy.Integer(0)
    x = sympy.symbols('x', real=True)
    u = sympy.symbols('u', cls=sympy.Function)
    curf = f
    res += eta
    for i in range(1, n_iterations + 1):
        # Calculate next summand of Taylor series: u^(i)(\xi) / i! * (x - \xi)^i
        exp = curf.subs([(u(x), eta), (x, xi)])
        exp *= (x - xi)**i / sympy.factorial(i)
        res += exp
        # Calculate next derivative and replace all occurences of u'(x) with f.
        curf = sympy.diff(curf, x).subs(sympy.Derivative(u(x), x), f).expand()
    return res
