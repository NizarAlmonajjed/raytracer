#pragma once

#include <cmath>
#include <cstdint>
#include <random>

#include "rt/vec3.h"

namespace rt {

// The calling thread's random number generator.
//
// std::mt19937 is the standard Mersenne Twister generator. `thread_local`
// gives each thread its own generator: sharing one between threads would be
// a data race (and a lock around it would be slow).
inline std::mt19937& thread_rng() {
    thread_local std::mt19937 generator{42};
    return generator;
}

// Restarts the calling thread's generator from `seed`. The renderer reseeds
// at the start of every image row, so each row's random numbers are the same
// no matter which thread renders it: the output is identical for any thread
// count.
inline void seed_thread_rng(std::uint32_t seed) { thread_rng().seed(seed); }

// Returns a uniformly distributed random number in [0, 1).
inline double random_double() {
    // A distribution object is cheap to create; making a fresh one each call
    // means no hidden state survives a reseed.
    return std::uniform_real_distribution<double>{0.0, 1.0}(thread_rng());
}

// Returns a uniformly distributed random number in [min, max).
inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

// Returns a random direction, uniformly distributed over the unit sphere.
// Rejection sampling: pick random points in the cube [-1,1]^3 until one lands
// inside the unit ball, then scale it to length 1. (Points in the cube's
// corners would bias the directions toward the diagonals.)
inline Vec3 random_unit_vector() {
    while (true) {
        const Vec3 p{random_double(-1.0, 1.0), random_double(-1.0, 1.0),
                     random_double(-1.0, 1.0)};
        const double len_sq = p.length_squared();
        // The lower bound avoids dividing by a (nearly) zero length.
        if (len_sq > 1e-160 && len_sq <= 1.0) {
            return p / std::sqrt(len_sq);
        }
    }
}

}  // namespace rt
