#include <limits>

#include <gtest/gtest.h>

#include "rt/sphere.h"
#include "test_helpers.h"

using rt::Point3;
using rt::Ray;
using rt::Sphere;
using rt::Vec3;
using rt_test::VecNear;

namespace {
constexpr double kInf = std::numeric_limits<double>::infinity();
}

// A sphere of radius 0.5 centred one unit in front of the origin (down -z).
const Sphere kSphere{Point3{0, 0, -1}, 0.5};

// ---- intersect ---------------------------------------------------------------

TEST(Intersect, RayStraightAtSphereHitsFrontSurface) {
    const Ray r{Point3{0, 0, 0}, Vec3{0, 0, -1}};
    const auto t = rt::intersect(kSphere, r, 0.0, kInf);
    ASSERT_TRUE(t.has_value());
    EXPECT_DOUBLE_EQ(*t, 0.5);  // front surface is at z = -0.5
}

TEST(Intersect, RayPointingAwayMisses) {
    const Ray r{Point3{0, 0, 0}, Vec3{0, 1, 0}};
    EXPECT_FALSE(rt::intersect(kSphere, r, 0.0, kInf).has_value());
}

TEST(Intersect, SphereBehindRayIsNotHit) {
    // Both roots are negative: the sphere is behind the ray's origin.
    const Ray r{Point3{0, 0, 0}, Vec3{0, 0, 1}};
    EXPECT_FALSE(rt::intersect(kSphere, r, 0.0, kInf).has_value());
}

TEST(Intersect, RayFromInsideHitsFarSurface) {
    // Start at the centre: the near root is negative, so the far one is used.
    const Ray r{Point3{0, 0, -1}, Vec3{0, 0, 1}};
    const auto t = rt::intersect(kSphere, r, 0.0, kInf);
    ASSERT_TRUE(t.has_value());
    EXPECT_DOUBLE_EQ(*t, 0.5);
}

TEST(Intersect, TangentRayGrazesSphere) {
    // Travels along y = 0.5, touching the top of the sphere at one point.
    const Ray r{Point3{0, 0.5, 0}, Vec3{0, 0, -1}};
    const auto t = rt::intersect(kSphere, r, 0.0, kInf);
    ASSERT_TRUE(t.has_value());
    EXPECT_NEAR(*t, 1.0, 1e-6);
}

TEST(Intersect, HitBeyondTMaxIsIgnored) {
    const Ray r{Point3{0, 0, 0}, Vec3{0, 0, -1}};
    EXPECT_FALSE(rt::intersect(kSphere, r, 0.0, 0.4).has_value());
}

TEST(Intersect, WorksWithNonUnitDirection) {
    // Direction twice as long -> the same point is reached at half the t.
    const Ray r{Point3{0, 0, 0}, Vec3{0, 0, -2}};
    const auto t = rt::intersect(kSphere, r, 0.0, kInf);
    ASSERT_TRUE(t.has_value());
    EXPECT_DOUBLE_EQ(*t, 0.25);
    EXPECT_TRUE(VecNear(r.at(*t), Point3{0, 0, -0.5}));
}

// ---- Tests for your TODO: rt::sphere_normal ---------------------------------

TEST(SphereNormal, TopOfSpherePointsUp) {
    EXPECT_TRUE(VecNear(rt::sphere_normal(kSphere, Point3{0, 0.5, -1}), Vec3{0, 1, 0}));
}

TEST(SphereNormal, RightSidePointsRight) {
    EXPECT_TRUE(VecNear(rt::sphere_normal(kSphere, Point3{0.5, 0, -1}), Vec3{1, 0, 0}));
}

TEST(SphereNormal, FrontPointsTowardCamera) {
    EXPECT_TRUE(VecNear(rt::sphere_normal(kSphere, Point3{0, 0, -0.5}), Vec3{0, 0, 1}));
}

TEST(SphereNormal, IsUnitLengthAtAnySurfacePoint) {
    // Find a real surface point by shooting a diagonal ray at the sphere.
    const Ray r{Point3{0, 0, 0}, Vec3{0.2, 0.1, -1}};
    const auto t = rt::intersect(kSphere, r, 0.0, kInf);
    ASSERT_TRUE(t.has_value());
    const Vec3 n = rt::sphere_normal(kSphere, r.at(*t));
    EXPECT_NEAR(n.length(), 1.0, 1e-9);
}
