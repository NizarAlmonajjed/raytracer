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

## Milestone 6: bounding volume hierarchy (2026-11-10)

Same machine and build as above. `--no-bvh` renders with the plain object list
(every ray tests every object). The BVH and no-BVH images were compared and are
byte-for-byte identical. BVH build time is not included (it printed as 0.000 s).

### Random scene: 479 spheres, 400x225, 100 samples/pixel, max depth 50

| Threads | Acceleration | Runs (s)               | Median (s) | Speedup from BVH |
|--------:|--------------|------------------------|-----------:|-----------------:|
| 1       | none (list)  | 19.404, 19.763, 19.388 | 19.404     |                  |
| 1       | BVH          | 4.485, 4.407, 4.418    | 4.418      | 4.39x            |
| 8       | none (list)  | 4.360, 4.576, 4.704    | 4.576      |                  |
| 8       | BVH          | 0.871, 0.864, 0.862    | 0.864      | 5.30x            |

Combined, BVH + 8 threads is 22.5x faster than the single-threaded list
(19.404 s -> 0.864 s).

### Materials scene: 5 spheres, 400x225, 100 samples/pixel, max depth 50, 8 threads

| Acceleration | Runs (s)            | Median (s) |
|--------------|---------------------|-----------:|
| none (list)  | 0.344, 0.344, 0.344 | 0.344      |
| BVH          | 0.448, 0.456, 0.452 | 0.452      |

With only 5 objects the BVH is *slower* (about 1.3x): the extra box tests and
virtual calls cost more than simply testing 5 spheres. A BVH pays off when
there are many objects.

Why the gain on the random scene is ~5x rather than closer to
479 / log2(479): the ground is a sphere of radius 1000, so its bounding box
overlaps everything and it is tested near the root for every ray; and most
rays hit the ground or several small spheres along their bounces.
