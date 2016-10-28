This code provides parallel implementation of matrix inversion using [Gaussian
elimination](https://en.wikipedia.org/wiki/Gaussian_elimination) algorithm.

Run with the following command:

```bash
mpirun -n PROCESSES_NUM ./inversion.py
```

Input matrix is read from 'matrix.txt', output is written to
'inversed-matrix.txt'.
