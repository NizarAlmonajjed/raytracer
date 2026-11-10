#pragma once

#include <cmath>
#include <random>

#include "rt/vec3.h"

namespace rt {

// Returns a uniformly distributed random number in [0, 1).
//
// std::mt19937 is the standard Mersenne Twister generator. `thread_local`
// gives each thread its own generator, which will matter in milestone 5
// (sharing one generator between threads would be a data race). The fixed
// seed makes renders reproducible run to run.
inline double random_double() {
    thread_local std::mt19937 generator{42};
    thread_local std::uniform_real_distribution<double> distribution{0.0, 1.0};
    return distribution(generator);
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
