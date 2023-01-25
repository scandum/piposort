Intro
-----
This document describes a stable top-down adaptive branchless merge sort named piposort. It is intended as a simplified [quadsort](https://github.com/scandum/quadsort) with reduced adaptivity, but a great reduction in lines of code and overall complexity. The name stands for ping-pong.

Quad partitioning
-----------------
During the partitioning phase the array is split into 4 segments (instead of the traditional 2) until the segment size drops below 8 elements.

Adaptive oddeven sort
---------------------
Segments ranging in size from 2 to 7 elements are sorted with oddeven sort, which is a bubblesort variant with a better memory access pattern, which improves performance when combined with branchless operations.

The best case is 6 comparisons to sort 7 elements, the worst case is 21. Its overall performance is very good.

Branchless parity merge
-----------------------
Segments sorted with oddeven sort are ping-pong merged with branchless parity merges, 4 segments at a time, until the array is fully sorted.

Adaptiveness
------------
Between parity merges it is checked whether the four segments are in order or in reverse order. Reverse order segments are placed in their proper position using a simple auxiliary rotation.

Big O
-----
```
                 ┌───────────────────────┐┌───────────────────────┐
                 │comparisons            ││swap memory            │
┌───────────────┐├───────┬───────┬───────┤├───────┬───────┬───────┤┌──────┐┌─────────┐┌─────────┐
│name           ││min    │avg    │max    ││min    │avg    │max    ││stable││partition││adaptive │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│mergesort      ││n log n│n log n│n log n││n      │n      │n      ││yes   ││no       ││no       │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│quadsort       ││n      │n log n│n log n││1      │n      │n      ││yes   ││no       ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│piposort       ││n      │n log n│n log n││n      │n      │n      ││yes   ││no       ││semi     │
└───────────────┘└───────┴───────┴───────┘└───────┴───────┴───────┘└──────┘└─────────┘└─────────┘
```

Data Types
----------
The C implementation of piposort supports long doubles and 8, 16, 32, and 64 bit data types. By using pointers it's possible to sort any other data type, like strings.

Interface
---------
Piposort uses the same interface as qsort, which is described in [man qsort](https://man7.org/linux/man-pages/man3/qsort.3p.html).

Memory
------
Piposort requires n memory.

Performance
-----------
For random data piposort is one of the fastest sorts available. There is no adaptivity for generic data, so [fluxsort](https://github.com/scandum/fluxsort) would outperform piposort for random data with low cardinality.

To take full advantage of branchless operations the cmp macro needs to be uncommented in bench.c, which will increase the performance by 30% on primitive types. This mode only allows sorting arrays of unsigned primitives by default.

The code is optimized for gcc and needs to be compiled with the -O3 option for proper performance.

Benchmark: piposort vs qsort (mergesort)
----------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04).
The source code was compiled using gcc -O3 bench.c. Each test was ran 100 times. It's generated
by running the benchmark using 100000 100 1 as the argument. In the benchmark piposort is
compared against glibc qsort() using the same general purpose interface and without any known
unfair advantage, like inlining.

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   64 | 0.016855 | 0.017032 |   1536381 |     100 |    random string |
|  piposort |   100000 |   64 | 0.010892 | 0.011019 |   1638620 |     100 |    random string |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |  128 | 0.019067 | 0.019959 |   1536491 |     100 |     random order |
|  piposort |   100000 |  128 | 0.011862 | 0.011926 |   1638209 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   64 | 0.009377 | 0.009448 |   1536491 |     100 |     random order |
|  piposort |   100000 |   64 | 0.004036 | 0.004053 |   1638209 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   32 | 0.008947 | 0.009087 |   1536634 |     100 |     random order |
|  piposort |   100000 |   32 | 0.003334 | 0.003392 |   1638867 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.006876 | 0.007038 |   1532324 |     100 |     random % 100 |
|  piposort |   100000 |   32 | 0.003337 | 0.003452 |   1637919 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002608 | 0.002667 |    815024 |     100 |  ascending order |
|  piposort |   100000 |   32 | 0.000279 | 0.000292 |     99999 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003391 | 0.003574 |    915020 |     100 |    ascending saw |
|  piposort |   100000 |   32 | 0.000648 | 0.000671 |    299998 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002665 | 0.002681 |    884462 |     100 |       pipe organ |
|  piposort |   100000 |   32 | 0.000932 | 0.000975 |    388889 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002455 | 0.002555 |    853904 |     100 | descending order |
|  piposort |   100000 |   32 | 0.000879 | 0.000903 |    277780 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003292 | 0.003357 |    953892 |     100 |   descending saw |
|  piposort |   100000 |   32 | 0.001214 | 0.001300 |    477778 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.004234 | 0.004421 |   1012003 |     100 |      random tail |
|  piposort |   100000 |   32 | 0.001318 | 0.001352 |    634685 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.005966 | 0.006083 |   1200707 |     100 |      random half |
|  piposort |   100000 |   32 | 0.001991 | 0.002015 |    969386 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.004350 | 0.004434 |   1209200 |     100 |  ascending tiles |
|  piposort |   100000 |   32 | 0.005177 | 0.005368 |   1573354 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.005221 | 0.005843 |   1553378 |     100 |     bit reversal |
|  piposort |   100000 |   32 | 0.003257 | 0.003367 |   1626250 |     100 |     bit reversal |
