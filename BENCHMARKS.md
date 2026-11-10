# Benchmarks

All numbers below were measured on the author's machine. Each configuration was
run 3 times; the table shows the median. Only the render is timed (not scene
setup or writing the file), using `std::chrono::steady_clock`.

**Machine:** Apple M3 (4 performance + 4 efficiency cores), macOS, Apple clang 21
**Build:** CMake `RelWithDebInfo` (`-O2 -g`)
**Scene:** materials scene (ground, hollow glass ball, matte ball, fuzzy metal ball; 5 spheres), max depth 50

## Milestone 5: multithreading (2026-11-10)

Rows are handed out dynamically from a shared `std::atomic<int>` counter.
Output is bit-identical for every thread count.

### 400x225, 100 samples/pixel

| Threads | Runs (s)            | Median (s) | Speedup vs 1 thread |
|--------:|---------------------|-----------:|--------------------:|
| 1       | 1.757, 1.804, 1.874 | 1.804      | 1.00x               |
| 2       | 0.922, 0.944, 0.926 | 0.926      | 1.95x               |
| 4       | 0.478, 0.513, 0.485 | 0.485      | 3.72x               |
| 8       | 0.360, 0.351, 0.365 | 0.360      | 5.01x               |

### 800x450, 200 samples/pixel

| Threads | Runs (s)               | Median (s) | Speedup vs 1 thread |
|--------:|------------------------|-----------:|--------------------:|
| 1       | 14.529, 14.477, 14.505 | 14.505     | 1.00x               |
| 4       | 3.887, 3.973, 3.780    | 3.887      | 3.73x               |
| 8       | 2.782, 2.779, 2.835    | 2.782      | 5.21x               |

Going from 4 to 8 threads gives less than 2x because the extra 4 cores are the
M3's slower efficiency cores.
