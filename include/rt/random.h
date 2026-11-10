#pragma once

#include <random>

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

}  // namespace rt
