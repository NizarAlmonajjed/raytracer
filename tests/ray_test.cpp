#include <gtest/gtest.h>

#include "rt/ray.h"
#include "test_helpers.h"

using rt::Ray;
using rt::Vec3;
using rt_test::VecNear;

TEST(Ray, StoresOriginAndDirection) {
    const Ray r{Vec3{1, 2, 3}, Vec3{0, 0, -1}};
    EXPECT_TRUE(VecNear(r.origin(), Vec3{1, 2, 3}));
    EXPECT_TRUE(VecNear(r.direction(), Vec3{0, 0, -1}));
}

TEST(Ray, AtZeroIsOrigin) {
    const Ray r{Vec3{1, 2, 3}, Vec3{4, 5, 6}};
    EXPECT_TRUE(VecNear(r.at(0.0), Vec3{1, 2, 3}));
}

TEST(Ray, AtMovesAlongDirection) {
    const Ray r{Vec3{1, 0, 0}, Vec3{0, 2, 0}};
    EXPECT_TRUE(VecNear(r.at(1.5), Vec3{1, 3, 0}));
    EXPECT_TRUE(VecNear(r.at(-1.0), Vec3{1, -2, 0}));
}
