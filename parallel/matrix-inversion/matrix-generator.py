#!/usr/bin/python3
print('Please enter matrix size: ')
n = int(input())
with open('matrix.txt', 'w') as f:
    f.write('{}\n'.format(n))
    for i in range(0, n):
        for j in range(0, n):
            if i == j:
                f.write('{} '.format(10))
            else:
                f.write('{} '.format(1))
        f.write('\n')
