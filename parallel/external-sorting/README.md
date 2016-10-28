This is an implementation of
[external](https://en.wikipedia.org/wiki/External_sorting) Merge sort
algorithm.

Run with the following command:

```bash
mpirun -n PROCESSES_NUM ./external-sort
```

'external-sort.cpp' reads an array from 'array.txt' file an saves sorted array
to 'sorted-array.txt'.

'array-generator.cpp' generates an array of arbitrary length containing
integers in range [INT_MIN, INT_MAX].
