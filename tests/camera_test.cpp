#include <stdexcept>

#include <gtest/gtest.h>

#include "rt/camera.h"
#include "rt/hittable_list.h"
#include "rt/random.h"
#include "test_helpers.h"

using rt::Camera;
using rt::CameraSettings;
using rt::Point3;
using rt::Vec3;
using rt_test::VecNear;

TEST(Camera, HeightFollowsAspectRatio) {
    CameraSettings s;
    s.image_width = 400;
    s.aspect_ratio = 16.0 / 9.0;
    const Camera cam{s};
    EXPECT_EQ(cam.image_width(), 400);
    EXPECT_EQ(cam.image_height(), 225);
}

TEST(Camera, HeightIsAtLeastOne) {
    CameraSettings s;
    s.image_width = 1;
    s.aspect_ratio = 16.0 / 9.0;
    EXPECT_EQ(Camera{s}.image_height(), 1);
}

TEST(Camera, RejectsInvalidSettings) {
    CameraSettings s;
    s.image_width = 0;
    EXPECT_THROW(Camera{s}, std::invalid_argument);
    s.image_width = 100;
    s.samples_per_pixel = 0;
    EXPECT_THROW(Camera{s}, std::invalid_argument);
}

TEST(Camera, CentreRayPointsAtTarget) {
    CameraSettings s;
    s.look_from = Point3{1, 2, 3};
    s.look_at = Point3{4, 2, -1};  // distance 5 away
    const Camera cam{s};

    const rt::Ray r = cam.ray_through(0.5, 0.5);
    EXPECT_TRUE(VecNear(r.origin(), s.look_from));
    EXPECT_TRUE(VecNear(rt::unit_vector(r.direction()), Vec3{0.6, 0, -0.8}));
}

TEST(Camera, NinetyDegreeFovTopEdgeIsAt45Degrees) {
    // With a 90-degree vertical FOV, the ray through the middle of the top
    // edge points 45 degrees up: equal parts up and forward.
    CameraSettings s;
    s.vertical_fov_deg = 90.0;
    const Camera cam{s};
    const Vec3 d = rt::unit_vector(cam.ray_through(0.5, 0.0).direction());
    EXPECT_NEAR(d.y, -d.z, 1e-9);
    EXPECT_GT(d.y, 0.0);
}

TEST(Camera, RenderOfEmptyWorldIsSky) {
    CameraSettings s;
    s.image_width = 8;
    s.samples_per_pixel = 1;
    const Camera cam{s};
    const rt::HittableList empty;
    const rt::Image img = cam.render(empty);
    EXPECT_EQ(img.width(), 8);
    EXPECT_EQ(img.height(), cam.image_height());
    // Sky is bluer at the top than at the bottom.
    EXPECT_LT(img.at(4, 0).x, img.at(4, img.height() - 1).x);
}

TEST(RayColor, MissReturnsSkyGradient) {
    const rt::HittableList empty;
    // Straight up: fully sky blue.
    EXPECT_TRUE(VecNear(rt::ray_color(rt::Ray{Point3{}, Vec3{0, 1, 0}}, empty),
                        rt::Color{0.5, 0.7, 1.0}));
    // Straight down: white.
    EXPECT_TRUE(VecNear(rt::ray_color(rt::Ray{Point3{}, Vec3{0, -1, 0}}, empty),
                        rt::Color{1.0, 1.0, 1.0}));
}

TEST(Random, StaysInRange) {
    for (int i = 0; i < 10000; ++i) {
        const double r = rt::random_double();
        ASSERT_GE(r, 0.0);
        ASSERT_LT(r, 1.0);
        const double s = rt::random_double(-2.0, 3.0);
        ASSERT_GE(s, -2.0);
        ASSERT_LT(s, 3.0);
    }
}
