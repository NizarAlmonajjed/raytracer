#include <cmath>
#include <limits>
#include <memory>
#include <stdexcept>

#include <gtest/gtest.h>

#include "rt/aabb.h"
#include "rt/bvh.h"
#include "rt/hittable_list.h"
#include "rt/random.h"
#include "rt/scenes.h"
#include "rt/sphere.h"
#include "test_helpers.h"

using rt::AABB;
using rt::Point3;
using rt::Ray;
using rt::Vec3;
using rt_test::VecNear;

namespace {
constexpr double kInf = std::numeric_limits<double>::infinity();
const AABB kUnitBox{Point3{0, 0, 0}, Point3{1, 1, 1}};
}  // namespace

// ---- AABB -------------------------------------------------------------------

TEST(AABB, RayThroughBoxHits) {
    EXPECT_TRUE(kUnitBox.hit(Ray{Point3{0.5, 0.5, -5}, Vec3{0, 0, 1}}, 0.0, kInf));
}

TEST(AABB, RayBesideBoxMisses) {
    EXPECT_FALSE(kUnitBox.hit(Ray{Point3{2, 0.5, -5}, Vec3{0, 0, 1}}, 0.0, kInf));
}

TEST(AABB, RayPointingAwayMisses) {
    EXPECT_FALSE(kUnitBox.hit(Ray{Point3{0.5, 0.5, -5}, Vec3{0, 0, -1}}, 0.0, kInf));
}

TEST(AABB, DiagonalRayHits) {
    EXPECT_TRUE(kUnitBox.hit(Ray{Point3{-1, -1, -1}, Vec3{1, 1, 1}}, 0.0, kInf));
}

TEST(AABB, NegativeDirectionHits) {
    EXPECT_TRUE(kUnitBox.hit(Ray{Point3{5, 0.5, 0.5}, Vec3{-1, 0, 0}}, 0.0, kInf));
}

TEST(AABB, RespectsTInterval) {
    const Ray r{Point3{0.5, 0.5, -5}, Vec3{0, 0, 1}};  // inside for t in [5, 6]
    EXPECT_FALSE(kUnitBox.hit(r, 0.0, 4.0));
    EXPECT_FALSE(kUnitBox.hit(r, 7.0, kInf));
    EXPECT_TRUE(kUnitBox.hit(r, 5.5, 5.6));
}

TEST(AABB, RayLyingOnFaceDoesNotProduceGarbage) {
    // Origin exactly on the x = 0 plane, travelling along it: 0 * inf = NaN in
    // the slab maths. The test must still give a sensible answer.
    EXPECT_TRUE(kUnitBox.hit(Ray{Point3{0, 0.5, -5}, Vec3{0, 0, 1}}, 0.0, kInf));
}

TEST(AABB, SurroundingAndHelpers) {
    const AABB b = AABB::surrounding(kUnitBox, AABB{Point3{-1, 2, 0}, Point3{0, 3, 5}});
    EXPECT_TRUE(VecNear(b.min(), Point3{-1, 0, 0}));
    EXPECT_TRUE(VecNear(b.max(), Point3{1, 3, 5}));
    EXPECT_EQ(b.longest_axis(), 2);  // z spans 5
    EXPECT_TRUE(VecNear(kUnitBox.centroid(), Point3{0.5, 0.5, 0.5}));
}

TEST(AABB, EmptyBoxIsIdentityForSurrounding) {
    const AABB empty;
    EXPECT_TRUE(empty.is_empty());
    const AABB b = AABB::surrounding(empty, kUnitBox);
    EXPECT_TRUE(VecNear(b.min(), kUnitBox.min()));
    EXPECT_TRUE(VecNear(b.max(), kUnitBox.max()));
}

// ---- Bounding boxes of objects ----------------------------------------------

TEST(BoundingBox, SphereBox) {
    const rt::Sphere s{Point3{1, 2, 3}, 0.5};
    EXPECT_TRUE(VecNear(s.bounding_box().min(), Point3{0.5, 1.5, 2.5}));
    EXPECT_TRUE(VecNear(s.bounding_box().max(), Point3{1.5, 2.5, 3.5}));
}

TEST(BoundingBox, ListBoxGrowsWithObjects) {
    rt::HittableList list;
    EXPECT_TRUE(list.bounding_box().is_empty());
    list.add(std::make_shared<rt::Sphere>(Point3{0, 0, 0}, 1.0));
    list.add(std::make_shared<rt::Sphere>(Point3{5, 0, 0}, 1.0));
    EXPECT_TRUE(VecNear(list.bounding_box().min(), Point3{-1, -1, -1}));
    EXPECT_TRUE(VecNear(list.bounding_box().max(), Point3{6, 1, 1}));
    list.clear();
    EXPECT_TRUE(list.bounding_box().is_empty());
}

// ---- BVH --------------------------------------------------------------------

TEST(Bvh, RejectsEmptyInput) {
    EXPECT_THROW(rt::build_bvh({}), std::invalid_argument);
}

TEST(Bvh, SingleObjectIsItsOwnRoot) {
    const auto sphere = std::make_shared<rt::Sphere>(Point3{0, 0, -1}, 0.5);
    EXPECT_EQ(rt::build_bvh({sphere}), sphere);
}

TEST(Bvh, FindsClosestHit) {
    rt::HittableList list;
    for (int i = 0; i < 10; ++i) {
        list.add(std::make_shared<rt::Sphere>(Point3{0, 0, -3.0 * (i + 1)}, 1.0));
    }
    const auto bvh = rt::build_bvh(list.objects());
    const auto rec = bvh->hit(Ray{Point3{0, 0, 0}, Vec3{0, 0, -1}}, 0.0, kInf);
    ASSERT_TRUE(rec.has_value());
    EXPECT_DOUBLE_EQ(rec->t, 2.0);  // the nearest sphere's front at z = -2
}

TEST(Bvh, BoxContainsEverything) {
    const rt::Scene scene = rt::make_scene("random");
    const auto bvh = rt::build_bvh(scene.world.objects());
    EXPECT_TRUE(VecNear(bvh->bounding_box().min(), scene.world.bounding_box().min()));
    EXPECT_TRUE(VecNear(bvh->bounding_box().max(), scene.world.bounding_box().max()));
}

// The key correctness test: a BVH is only an optimisation, so it must give
// exactly the same answers as testing every object.
TEST(Bvh, AgreesWithListForManyRandomRays) {
    const rt::Scene scene = rt::make_scene("random");
    const auto bvh = rt::build_bvh(scene.world.objects());

    rt::seed_thread_rng(123);
    for (int i = 0; i < 20000; ++i) {
        const Point3 origin{rt::random_double(-12, 12), rt::random_double(0.1, 4),
                            rt::random_double(-12, 12)};
        const Ray ray{origin, rt::random_unit_vector()};

        const auto expected = scene.world.hit(ray, 0.001, kInf);
        const auto actual = bvh->hit(ray, 0.001, kInf);
        ASSERT_EQ(actual.has_value(), expected.has_value()) << "ray " << i;
        if (expected) {
            ASSERT_DOUBLE_EQ(actual->t, expected->t) << "ray " << i;
            ASSERT_EQ(actual->material, expected->material) << "ray " << i;
        }
    }
}

// ---- Scenes -----------------------------------------------------------------

TEST(Scenes, KnownScenesBuild) {
    EXPECT_EQ(rt::make_scene("materials").world.size(), 5u);
    EXPECT_GT(rt::make_scene("random").world.size(), 400u);
}

TEST(Scenes, RandomSceneIsTheSameEveryTime) {
    const rt::Scene a = rt::make_scene("random");
    const rt::Scene b = rt::make_scene("random");
    ASSERT_EQ(a.world.size(), b.world.size());
    EXPECT_TRUE(VecNear(a.world.bounding_box().min(), b.world.bounding_box().min()));
    EXPECT_TRUE(VecNear(a.world.bounding_box().max(), b.world.bounding_box().max()));
}

TEST(Scenes, UnknownSceneThrows) {
    EXPECT_THROW(rt::make_scene("teapot"), std::invalid_argument);
}
