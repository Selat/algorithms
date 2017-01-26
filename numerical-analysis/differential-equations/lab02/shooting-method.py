import math
import pylab
import numpy as np
import matplotlib.pyplot as plt

a, b = 1.0, 2.0
A, B = 0.0, 1.0
n = 20
h = (b - a) / n
s = 15
u_approx_x = np.zeros(1)
u_approx_y = np.zeros(1)
du_approx_x = np.zeros(1)
du_approx_y = np.zeros(1)

x = np.arange(a, b + 0.0001, h)

def realf(x):
    return (x ** 2 - 1) * (3 * s * (x ** 2 - 4) + 8) / 24.0

def find_nearest_id(array, value):
    return np.abs(array - value).argmin()

def du(x, y):
    return du_approx_y[find_nearest_id(du_approx_x, x)]

def ddu(x, y):
    return  y / x + s * x * x

def gety(x, y, h, f):
    k1 = h * f(x, y)
    k2 = h * f(x + h * 0.5, y + k1 * 0.5)
    k3 = h * f(x + h * 0.5, y + k2 * 0.5)
    k4 = h * f(x + h, y + k3)
    return y + (k1 + 2.0 * k2 + 2.0 * k3 + k4) / 6.0

def runge_kutta1(h):
    global u_approx_x, u_approx_y
    curx = a
    while curx < b and h > 0.0:
        y1 = gety(u_approx_x[-1], u_approx_y[-1], h, ddu)
        curx += h
        u_approx_x = np.append(u_approx_x, curx + h)
        u_approx_y = np.append(u_approx_y, y1)


def runge_kutta2(h):
    global du_approx_x, du_approx_y
    curx = a
    while curx < b and h > 0.0:
        y1 = gety(du_approx_x[-1], du_approx_y[-1], h, du)
        curx += h
        du_approx_x = np.append(du_approx_x, curx + h)
        du_approx_y = np.append(du_approx_y, y1)

h = 0.25
alpha = 1
newton_iterations = 0
u_approx_x = np.array([a])
u_approx_y = np.array([A])
du_approx_x = np.array([a])
du_approx_y = np.array([alpha])

runge_kutta1(h * 0.5)
runge_kutta2(h)
while np.abs(u_approx_y[-1] - 1.0) > 0.05:
    print(np.abs(u_approx_y[-1] - 1.0), alpha)
    newton_iterations += 1
    alpha = alpha - (u_approx_y[-1] - 1.0) / du_approx_y[-1]
    u_approx_x = np.array([a])
    u_approx_y = np.array([A])
    du_approx_x = np.array([a])
    du_approx_y = np.array([alpha])
    runge_kutta1(h * 0.5)
    runge_kutta2(h)
print(newton_iterations)
print('Error: {}'.format(np.max(np.abs(realf(x) - yy))))
