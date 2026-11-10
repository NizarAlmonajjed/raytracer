#include <memory>
#include <stdexcept>

#include <gtest/gtest.h>

#include "rt/camera.h"
#include "rt/hittable_list.h"
#include "rt/material.h"
#include "rt/sphere.h"

namespace {

// A small scene that exercises every material, so the random numbers matter.
rt::HittableList test_scene() {
    rt::HittableList world;
    world.add(std::make_shared<rt::Sphere>(
        rt::Point3{0, -100.5, -1}, 100.0,
        std::make_shared<rt::Lambertian>(rt::Color{0.5, 0.5, 0.5})));
    world.add(std::make_shared<rt::Sphere>(rt::Point3{-1, 0, -1}, 0.5,
                                           std::make_shared<rt::Dielectric>(1.5)));
    world.add(std::make_shared<rt::Sphere>(
        rt::Point3{1, 0, -1}, 0.5,
        std::make_shared<rt::Metal>(rt::Color{0.8, 0.6, 0.2}, 0.5)));
    return world;
}

rt::Camera small_camera() {
    rt::CameraSettings s;
    s.image_width = 48;  // 48x27
    s.samples_per_pixel = 8;
    s.max_depth = 10;
    return rt::Camera{s};
}

// Exact comparison on purpose: same seeds must give bit-identical results.
void expect_identical(const rt::Image& a, const rt::Image& b) {
    ASSERT_EQ(a.width(), b.width());
    ASSERT_EQ(a.height(), b.height());
    for (int y = 0; y < a.height(); ++y) {
        for (int x = 0; x < a.width(); ++x) {
            ASSERT_EQ(a.at(x, y).x, b.at(x, y).x) << "pixel " << x << "," << y;
            ASSERT_EQ(a.at(x, y).y, b.at(x, y).y) << "pixel " << x << "," << y;
            ASSERT_EQ(a.at(x, y).z, b.at(x, y).z) << "pixel " << x << "," << y;
        }
    }
}

}  // namespace

TEST(Threading, SameImageForAnyThreadCount) {
    const rt::HittableList world = test_scene();
    const rt::Camera cam = small_camera();
    const rt::Image one = cam.render(world, 1);
    expect_identical(one, cam.render(world, 2));
    expect_identical(one, cam.render(world, 4));
    expect_identical(one, cam.render(world, 7));  // doesn't divide the row count
}

TEST(Threading, RenderIsRepeatable) {
    const rt::HittableList world = test_scene();
    const rt::Camera cam = small_camera();
    expect_identical(cam.render(world, 4), cam.render(world, 4));
}

TEST(Threading, MoreThreadsThanRowsIsFine) {
    const rt::HittableList world = test_scene();
    const rt::Camera cam = small_camera();  // 27 rows
    expect_identical(cam.render(world, 1), cam.render(world, 100));
}

TEST(Threading, RejectsZeroThreads) {
    const rt::HittableList world = test_scene();
    EXPECT_THROW(small_camera().render(world, 0), std::invalid_argument);
}
