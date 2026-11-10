#include "rt/scenes.h"

#include <memory>
#include <stdexcept>

#include "rt/material.h"
#include "rt/random.h"
#include "rt/sphere.h"

namespace rt {

namespace {

Scene materials_scene() {
    // Each material is created once and shared by every sphere that uses it.
    const auto ground = std::make_shared<Lambertian>(Color{0.8, 0.8, 0.0});
    const auto matte_blue = std::make_shared<Lambertian>(Color{0.1, 0.2, 0.5});
    const auto glass = std::make_shared<Dielectric>(1.5);
    // An "air bubble" inside the glass makes it a hollow glass ball.
    const auto bubble = std::make_shared<Dielectric>(1.0 / 1.5);
    const auto gold = std::make_shared<Metal>(Color{0.8, 0.6, 0.2}, 0.3);

    Scene scene;
    scene.world.add(std::make_shared<Sphere>(Point3{0.0, -100.5, -1.0}, 100.0, ground));
    scene.world.add(std::make_shared<Sphere>(Point3{0.0, 0.0, -1.2}, 0.5, matte_blue));
    scene.world.add(std::make_shared<Sphere>(Point3{-1.0, 0.0, -1.0}, 0.5, glass));
    scene.world.add(std::make_shared<Sphere>(Point3{-1.0, 0.0, -1.0}, 0.4, bubble));
    scene.world.add(std::make_shared<Sphere>(Point3{1.0, 0.0, -1.0}, 0.5, gold));
    // Camera: the defaults (at the origin, looking down -z, 90 degree FOV).
    return scene;
}

Scene random_scene() {
    // Fixed seed: the "random" layout is identical on every run and machine,
    // so benchmarks always measure the same scene.
    seed_thread_rng(2026);

    Scene scene;
    scene.world.add(std::make_shared<Sphere>(
        Point3{0.0, -1000.0, 0.0}, 1000.0,
        std::make_shared<Lambertian>(Color{0.45, 0.5, 0.45})));

    // Three large feature spheres, in a row across the camera's view (the
    // row direction (0.385, 0, -0.923) is perpendicular to where it looks).
    const Point3 glass_center{0.0, 1.0, 0.0};
    const Point3 metal_center{-1.15, 1.0, 2.77};
    const Point3 matte_center{1.15, 1.0, -2.77};
    scene.world.add(std::make_shared<Sphere>(glass_center, 1.0,
                                             std::make_shared<Dielectric>(1.5)));
    scene.world.add(std::make_shared<Sphere>(
        metal_center, 1.0, std::make_shared<Metal>(Color{0.85, 0.85, 0.9}, 0.0)));
    scene.world.add(std::make_shared<Sphere>(
        matte_center, 1.0, std::make_shared<Lambertian>(Color{0.7, 0.15, 0.1})));

    // A jittered 22 x 22 grid of small spheres on the ground.
    for (int i = -11; i < 11; ++i) {
        for (int j = -11; j < 11; ++j) {
            const double radius = 0.2;
            const Point3 center{i + 0.8 * random_double(), radius, j + 0.8 * random_double()};

            // Keep clear of the big spheres.
            const bool too_close = (center - glass_center).length() < 1.25 ||
                                   (center - metal_center).length() < 1.25 ||
                                   (center - matte_center).length() < 1.25;
            const double pick = random_double();
            if (too_close) {
                continue;
            }

            std::shared_ptr<Material> material;
            if (pick < 0.7) {
                // Multiplying two random colours biases toward darker,
                // richer tones.
                const Color a{random_double(), random_double(), random_double()};
                const Color b{random_double(), random_double(), random_double()};
                material = std::make_shared<Lambertian>(a * b);
            } else if (pick < 0.9) {
                const Color albedo{random_double(0.5, 1.0), random_double(0.5, 1.0),
                                   random_double(0.5, 1.0)};
                material = std::make_shared<Metal>(albedo, random_double(0.0, 0.4));
            } else {
                material = std::make_shared<Dielectric>(1.5);
            }
            scene.world.add(std::make_shared<Sphere>(center, radius, std::move(material)));
        }
    }

    scene.camera.look_from = Point3{12.0, 2.2, 5.0};
    scene.camera.look_at = Point3{0.0, 0.6, 0.0};
    scene.camera.vertical_fov_deg = 25.0;
    return scene;
}

}  // namespace

std::vector<std::string> scene_names() { return {"materials", "random"}; }

Scene make_scene(const std::string& name) {
    if (name == "materials") {
        return materials_scene();
    }
    if (name == "random") {
        return random_scene();
    }
    throw std::invalid_argument("unknown scene '" + name + "'");
}

}  // namespace rt
