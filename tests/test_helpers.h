#pragma once

#include <cmath>

#include <gtest/gtest.h>

#include "rt/vec3.h"

namespace rt_test {

// Floating-point math is rarely exact, so compare vectors with a tolerance.
// Returning an AssertionResult gives a readable message when the test fails:
//   EXPECT_TRUE(VecNear(actual, expected));
inline ::testing::AssertionResult VecNear(const rt::Vec3& actual,
                                          const rt::Vec3& expected,
                                          double eps = 1e-9) {
    if (std::abs(actual.x - expected.x) <= eps &&
        std::abs(actual.y - expected.y) <= eps &&
        std::abs(actual.z - expected.z) <= eps) {
        return ::testing::AssertionSuccess();
    }
    return ::testing::AssertionFailure()
           << "got " << actual << ", expected " << expected << " (eps " << eps << ")";
}

}  // namespace rt_test
