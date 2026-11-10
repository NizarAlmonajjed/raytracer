#include <limits>
#include <memory>
#include <stdexcept>

#include <gtest/gtest.h>

#include "rt/hittable.h"
#include "rt/hittable_list.h"
#include "rt/sphere.h"
#include "test_helpers.h"

using rt::HitRecord;
using rt::HittableList;
using rt::Point3;
using rt::Ray;
using rt::Sphere;
using rt::Vec3;
using rt_test::VecNear;

namespace {
constexpr double kInf = std::numeric_limits<double>::infinity();
}

// ---- Sphere as a Hittable ---------------------------------------------------

TEST(SphereHit, RejectsNonPositiveRadius) {
    EXPECT_THROW(Sphere(Point3{0, 0, 0}, 0.0), std::invalid_argument);
    EXPECT_THROW(Sphere(Point3{0, 0, 0}, -1.0), std::invalid_argument);
}

TEST(SphereHit, RecordsTAndPoint) {
    const Sphere s{Point3{0, 0, -1}, 0.5};
    const auto rec = s.hit(Ray{Point3{0, 0, 0}, Vec3{0, 0, -1}}, 0.0, kInf);
    ASSERT_TRUE(rec.has_value());
    EXPECT_DOUBLE_EQ(rec->t, 0.5);
    EXPECT_TRUE(VecNear(rec->point, Point3{0, 0, -0.5}));
}

TEST(SphereHit, CallThroughBaseClassReferenceUsesSphereHit) {
    // The static type is Hittable&, but the virtual call reaches Sphere::hit.
    const Sphere s{Point3{0, 0, -1}, 0.5};
    const rt::Hittable& h = s;
    EXPECT_TRUE(h.hit(Ray{Point3{0, 0, 0}, Vec3{0, 0, -1}}, 0.0, kInf).has_value());
}

// ---- HittableList -----------------------------------------------------------

TEST(HittableList, EmptyListNeverHits) {
    const HittableList world;
    EXPECT_FALSE(world.hit(Ray{Point3{0, 0, 0}, Vec3{0, 0, -1}}, 0.0, kInf).has_value());
}

TEST(HittableList, ReturnsClosestHitRegardlessOfOrder) {
    // Far sphere added first, near sphere second: the near one must win.
    HittableList world;
    world.add(std::make_shared<Sphere>(Point3{0, 0, -10}, 1.0));
    world.add(std::make_shared<Sphere>(Point3{0, 0, -3}, 1.0));
    EXPECT_EQ(world.size(), 2u);

    const auto rec = world.hit(Ray{Point3{0, 0, 0}, Vec3{0, 0, -1}}, 0.0, kInf);
    ASSERT_TRUE(rec.has_value());
    EXPECT_DOUBLE_EQ(rec->t, 2.0);  // front of the near sphere at z = -2
}

TEST(HittableList, RespectsTMax) {
    HittableList world;
    world.add(std::make_shared<Sphere>(Point3{0, 0, -10}, 1.0));
    EXPECT_FALSE(world.hit(Ray{Point3{0, 0, 0}, Vec3{0, 0, -1}}, 0.0, 5.0).has_value());
}

TEST(HittableList, ClearRemovesEverything) {
    HittableList world;
    world.add(std::make_shared<Sphere>(Point3{0, 0, -3}, 1.0));
    world.clear();
    EXPECT_EQ(world.size(), 0u);
    EXPECT_FALSE(world.hit(Ray{Point3{0, 0, 0}, Vec3{0, 0, -1}}, 0.0, kInf).has_value());
}

// ---- Tests for your TODO: HitRecord::set_face_normal ------------------------

TEST(FaceNormal, RayFromOutsideIsFrontFace) {
    // Ray travels down -z and hits a surface whose outward normal is +z.
    HitRecord rec;
    rec.set_face_normal(Ray{Point3{0, 0, 0}, Vec3{0, 0, -1}}, Vec3{0, 0, 1});
    EXPECT_TRUE(rec.front_face);
    EXPECT_TRUE(VecNear(rec.normal, Vec3{0, 0, 1}));
}

TEST(FaceNormal, RayFromInsideFlipsNormal) {
    // Ray travels in the SAME direction as the outward normal: it's exiting.
    HitRecord rec;
    rec.set_face_normal(Ray{Point3{0, 0, 0}, Vec3{0, 0, 1}}, Vec3{0, 0, 1});
    EXPECT_FALSE(rec.front_face);
    EXPECT_TRUE(VecNear(rec.normal, Vec3{0, 0, -1}));
}

TEST(FaceNormal, SphereHitFromOutsideHasOutwardNormal) {
    const Sphere s{Point3{0, 0, -1}, 0.5};
    const auto rec = s.hit(Ray{Point3{0, 0, 0}, Vec3{0, 0, -1}}, 0.0, kInf);
    ASSERT_TRUE(rec.has_value());
    EXPECT_TRUE(rec->front_face);
    EXPECT_TRUE(VecNear(rec->normal, Vec3{0, 0, 1}));
}

TEST(FaceNormal, SphereHitFromInsideHasInwardNormal) {
    const Sphere s{Point3{0, 0, -1}, 0.5};
    const auto rec = s.hit(Ray{Point3{0, 0, -1}, Vec3{0, 0, -1}}, 0.0, kInf);
    ASSERT_TRUE(rec.has_value());
    EXPECT_FALSE(rec->front_face);
    EXPECT_TRUE(VecNear(rec->normal, Vec3{0, 0, 1}));  // flipped to face the ray
}
