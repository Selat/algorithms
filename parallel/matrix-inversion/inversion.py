#!/usr/bin/python3
from mpi4py import MPI
import numpy as np
import sys

'''
This code provides parallel implementation of matrix inversion using gaussian
elimination algorithm.
https://en.wikipedia.org/wiki/Gaussian_elimination

Matrix rows are distributed among processes as follows:
row_id | block_id|
-----------------|
   0   |    0    |
  ...  |   ...   |
(p - 1)| (p - 1) |
   p   |    0    |
 p + 1 |    1    |
  ...  |   ...   |
------------------
This scheme ensures that all processes are equally busy during the
elimination process.
'''

def get_block_size(id, p, n):
    return ((n + p - id - 1) // p, n)

def gauss(rows, comm):
    n = rows.shape[1]
    rev_rows = np.zeros(rows.shape, dtype=np.float64)
    for i in range(0, rows.shape[0]):
        rev_rows[i,comm.rank + i * comm.Get_size()] = 1
    sid = 0
    curr = np.zeros(rows.shape[1], dtype=np.float64)
    curr2 = np.zeros(rows.shape[1], dtype=np.float64)
    # Convert matrix to triangular form.
    for cur_id in range(0, n):
        if cur_id % comm.Get_size() == comm.rank:
            row_id = cur_id // comm.Get_size()
            rev_rows[row_id] /= rows[row_id,cur_id]
            rows[row_id] /= rows[row_id,cur_id]
            comm.Bcast([rows[row_id], MPI.DOUBLE], root=comm.rank)
            comm.Bcast([rev_rows[row_id], MPI.DOUBLE], root=comm.rank)
            sid += 1
            for i in range(sid, rows.shape[0]):
                rev_rows[i] -= rev_rows[row_id] * rows[i, cur_id]
                rows[i] -= rows[row_id] * rows[i, cur_id]
        else:
            comm.Bcast([curr, MPI.DOUBLE], root=(cur_id % comm.Get_size()))
            comm.Bcast([curr2, MPI.DOUBLE], root=(cur_id % comm.Get_size()))
            for i in range(sid, rows.shape[0]):
                rev_rows[i] -= curr2 * rows[i, cur_id]
                rows[i] -= curr * rows[i, cur_id]
        comm.Barrier()
    sid = rows.shape[0]
    # Convert matrix from triangular to diagonal form.
    for cur_id in range(n - 1, -1, -1):
        if cur_id % comm.Get_size() == comm.rank:
            row_id = cur_id // comm.Get_size()
            comm.Bcast([rows[row_id], MPI.DOUBLE], root=comm.rank)
            comm.Bcast([rev_rows[row_id], MPI.DOUBLE], root=comm.rank)
            sid -= 1
            for i in range(0, sid):
                rev_rows[i] -= rev_rows[row_id] * rows[i, cur_id]
                rows[i] -= rows[row_id] * rows[i, cur_id]
        else:
            comm.Bcast([curr, MPI.DOUBLE], root=(cur_id % comm.Get_size()))
            comm.Bcast([curr2, MPI.DOUBLE], root=(cur_id % comm.Get_size()))
            for i in range(0, sid):
                rev_rows[i] -= curr2 * rows[i, cur_id]
                rows[i] -= curr * rows[i, cur_id]
        comm.Barrier()

    return rev_rows

comm = MPI.COMM_WORLD
rows = []
if comm.rank == 0:
    n = 0
    with open('matrix.txt') as f:
        n = int(f.readline())
        comm.bcast(n)
        for i in range(0, n):
            row = [int(a) for a in f.readline().split()]
            slave_id = i % comm.size
            if slave_id == 0:
                rows.append(row)
            else:
                comm.send(row, slave_id)
    rows = np.asarray(rows, dtype=np.float64)

    # Run inversion in multiple processes.
    r = gauss(rows, comm)

    # Combine and save data.
    data = np.zeros((n, n), dtype=np.float64)
    for j in range(0, r.shape[0]):
        data[j * comm.Get_size()] = r[j]
    for i in range(1, comm.Get_size()):
        comm.bcast(i)
        tmp = comm.recv()
        for j in range(0, tmp.shape[0]):
            data[i + j * comm.Get_size()] = tmp[j]
    with open('inversed-matrix.txt', 'w') as f:
        f.write('{}\n'.format(n))
        for r in data:
            for e in r:
                f.write('{} '.format(e))
            f.write('\n')
else:
    # Receive data from the master process.
    n = comm.bcast(None)
    ll = get_block_size(comm.rank, comm.Get_size(), n)[0]
    for i in range(0, ll):
        row = comm.recv()
        rows.append(row)
    rows = np.asarray(rows, dtype=np.float64)

    # Run inversion.
    r = gauss(rows, comm)

    # Send data to the master process.
    for i in range(1, comm.Get_size()):
        id = comm.bcast(None)
        if id == comm.rank:
            comm.send(r, 0)
