# highPrec
A Wrapper against the Multiprecision Floating Point library

https://www.mpfr.org/

Prerequisites: 

- Mpfr library
- Eigen Library

Optional:

- openmp

Link against the library mpfr, adding: ```-lmpfr```

Example:

```
g++ -o test test.cpp -lmpfr -fopenmp
```
