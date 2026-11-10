#include <cmath>
#include <memory>

#include <gtest/gtest.h>

#include "rt/camera.h"
#include "rt/hittable_list.h"
#include "rt/material.h"
#include "rt/random.h"
#include "rt/sphere.h"
#include "rt/util.h"
#include "test_helpers.h"

using rt::Color;
using rt::HitRecord;
using rt::Point3;
using rt::Ray;
using rt::Vec3;
using rt_test::VecNear;

namespace {

// A hit on the top of a flat surface at the origin: outward normal +y.
HitRecord hit_on_floor(const Ray& ray) {
    HitRecord rec;
    rec.point = Point3{0, 0, 0};
    rec.t = 1.0;
    rec.set_face_normal(ray, Vec3{0, 1, 0});
    return rec;
}

}  // namespace

// ---- Helpers ----------------------------------------------------------------

TEST(RandomUnitVector, HasLengthOne) {
    for (int i = 0; i < 1000; ++i) {
        ASSERT_NEAR(rt::random_unit_vector().length(), 1.0, 1e-9);
    }
}

TEST(NearZero, DetectsTinyVectors) {
    EXPECT_TRUE(Vec3(1e-9, -1e-9, 0).near_zero());
    EXPECT_FALSE(Vec3(0, 0, 1e-3).near_zero());
}

TEST(Refract, StraightThroughWhenIndicesMatch) {
    const Vec3 d = rt::unit_vector(Vec3{1, -1, 0});
    EXPECT_TRUE(VecNear(rt::refract(d, Vec3{0, 1, 0}, 1.0), d));
}

TEST(Refract, ObeysSnellsLaw) {
    // 45 degrees from air (1.0) into glass (1.5): sin(out) = sin(45) / 1.5.
    const Vec3 in = rt::unit_vector(Vec3{1, -1, 0});
    const Vec3 out = rt::refract(in, Vec3{0, 1, 0}, 1.0 / 1.5);
    EXPECT_NEAR(out.length(), 1.0, 1e-9);
    EXPECT_NEAR(out.x, std::sin(rt::kPi / 4) / 1.5, 1e-9);  // sideways part
    EXPECT_LT(out.y, 0.0);                                    // still going down
}

// ---- Lambertian -------------------------------------------------------------

TEST(Lambertian, AlwaysScattersAboveSurfaceWithAlbedo) {
    const rt::Lambertian mat{Color{0.2, 0.4, 0.6}};
    const Ray in{Point3{0, 1, 0}, Vec3{0, -1, 0}};
    const HitRecord rec = hit_on_floor(in);
    for (int i = 0; i < 1000; ++i) {
        const auto s = mat.scatter(in, rec);
        ASSERT_TRUE(s.has_value());
        EXPECT_TRUE(VecNear(s->attenuation, Color{0.2, 0.4, 0.6}));
        EXPECT_TRUE(VecNear(s->scattered.origin(), rec.point));
        ASSERT_GE(rt::dot(s->scattered.direction(), rec.normal), 0.0);
    }
}

// ---- Dielectric -------------------------------------------------------------

TEST(Dielectric, ClearGlassDoesNotTint) {
    const rt::Dielectric glass{1.5};
    const Ray in{Point3{0, 1, 0}, Vec3{0, -1, 0}};
    const auto s = glass.scatter(in, hit_on_floor(in));
    ASSERT_TRUE(s.has_value());
    EXPECT_TRUE(VecNear(s->attenuation, Color{1, 1, 1}));
}

TEST(Dielectric, IndexOneLetsLightStraightThrough) {
    // Same index as air: no bending, and reflectance is 0, so always refracts.
    const rt::Dielectric air{1.0};
    const Ray in{Point3{-1, 1, 0}, rt::unit_vector(Vec3{1, -1, 0})};
    const auto s = air.scatter(in, hit_on_floor(in));
    ASSERT_TRUE(s.has_value());
    EXPECT_TRUE(VecNear(s->scattered.direction(), in.direction()));
}

TEST(Dielectric, ReflectanceIsFourPercentHeadOnAndTotalAtGrazing) {
    // Glass seen straight on reflects about 4% of light.
    EXPECT_NEAR(rt::Dielectric::reflectance(1.0, 1.0 / 1.5), 0.04, 1e-9);
    // At a grazing angle (cosine 0) everything reflects.
    EXPECT_NEAR(rt::Dielectric::reflectance(0.0, 1.0 / 1.5), 1.0, 1e-9);
}

// ---- ray_color with bounces -------------------------------------------------

TEST(RayColor, NoBouncesLeftIsBlack) {
    const rt::HittableList empty;
    EXPECT_TRUE(VecNear(rt::ray_color(Ray{Point3{}, Vec3{0, 1, 0}}, empty, 0),
                        Color{0, 0, 0}));
}

TEST(RayColor, SurfaceWithoutMaterialIsBlack) {
    rt::HittableList world;
    world.add(std::make_shared<rt::Sphere>(Point3{0, 0, -1}, 0.5));
    EXPECT_TRUE(VecNear(rt::ray_color(Ray{Point3{}, Vec3{0, 0, -1}}, world, 10),
                        Color{0, 0, 0}));
}

TEST(RayColor, BlackMatteSurfaceAbsorbsEverything) {
    rt::HittableList world;
    world.add(std::make_shared<rt::Sphere>(
        Point3{0, 0, -1}, 0.5, std::make_shared<rt::Lambertian>(Color{0, 0, 0})));
    EXPECT_TRUE(VecNear(rt::ray_color(Ray{Point3{}, Vec3{0, 0, -1}}, world, 10),
                        Color{0, 0, 0}));
}

// ---- Gamma ------------------------------------------------------------------

TEST(Gamma, IsSquareRootAndClampsNegatives) {
    EXPECT_DOUBLE_EQ(rt::linear_to_gamma(0.25), 0.5);
    EXPECT_DOUBLE_EQ(rt::linear_to_gamma(1.0), 1.0);
    EXPECT_DOUBLE_EQ(rt::linear_to_gamma(0.0), 0.0);
    EXPECT_DOUBLE_EQ(rt::linear_to_gamma(-0.5), 0.0);
}

// ---- Tests for your TODO: rt::reflect ---------------------------------------

TEST(Reflect, BouncesOffFloor) {
    EXPECT_TRUE(VecNear(rt::reflect(Vec3{1, -1, 0}, Vec3{0, 1, 0}), Vec3{1, 1, 0}));
}

TEST(Reflect, StraightDownComesStraightBack) {
    EXPECT_TRUE(VecNear(rt::reflect(Vec3{0, -2, 0}, Vec3{0, 1, 0}), Vec3{0, 2, 0}));
}

TEST(Reflect, PreservesLength) {
    const Vec3 v{0.3, -0.7, 0.2};
    const Vec3 n = rt::unit_vector(Vec3{1, 2, 3});
    EXPECT_NEAR(rt::reflect(v, n).length(), v.length(), 1e-9);
}

TEST(MetalReflect, PerfectMirrorReflectsExactly) {
    const rt::Metal mirror{Color{0.9, 0.9, 0.9}, 0.0};
    const Ray in{Point3{-1, 1, 0}, Vec3{1, -1, 0}};
    const auto s = mirror.scatter(in, hit_on_floor(in));
    ASSERT_TRUE(s.has_value());
    EXPECT_TRUE(VecNear(s->attenuation, Color{0.9, 0.9, 0.9}));
    EXPECT_TRUE(VecNear(s->scattered.direction(), rt::unit_vector(Vec3{1, 1, 0})));
}

TEST(MetalReflect, DielectricTotalInternalReflection) {
    // A ray INSIDE glass, hitting the surface at a shallow angle, can't get
    // out (Snell's law has no solution) and must reflect back in.
    const rt::Dielectric glass{1.5};
    const Ray in{Point3{0, -1, 0}, rt::unit_vector(Vec3{1, 0.2, 0})};
    const HitRecord rec = hit_on_floor(in);  // ray going up: hits from inside
    ASSERT_FALSE(rec.front_face);

    const auto s = glass.scatter(in, rec);
    ASSERT_TRUE(s.has_value());
    EXPECT_TRUE(VecNear(s->scattered.direction(), rt::unit_vector(Vec3{1, -0.2, 0})));
}
