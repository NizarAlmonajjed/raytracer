#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "rt/camera.h"
#include "rt/hittable_list.h"
#include "rt/image.h"
#include "rt/material.h"
#include "rt/options.h"
#include "rt/sphere.h"

namespace {

// Ground, a hollow glass ball, a matte ball and a fuzzy gold ball.
rt::HittableList build_scene() {
    // Each material is created once and shared by every sphere that uses it.
    const auto ground = std::make_shared<rt::Lambertian>(rt::Color{0.8, 0.8, 0.0});
    const auto matte_blue = std::make_shared<rt::Lambertian>(rt::Color{0.1, 0.2, 0.5});
    const auto glass = std::make_shared<rt::Dielectric>(1.5);
    // An "air bubble" inside the glass makes it a hollow glass ball.
    const auto bubble = std::make_shared<rt::Dielectric>(1.0 / 1.5);
    const auto gold = std::make_shared<rt::Metal>(rt::Color{0.8, 0.6, 0.2}, 0.3);

    rt::HittableList world;
    world.add(std::make_shared<rt::Sphere>(rt::Point3{0.0, -100.5, -1.0}, 100.0, ground));
    world.add(std::make_shared<rt::Sphere>(rt::Point3{0.0, 0.0, -1.2}, 0.5, matte_blue));
    world.add(std::make_shared<rt::Sphere>(rt::Point3{-1.0, 0.0, -1.0}, 0.5, glass));
    world.add(std::make_shared<rt::Sphere>(rt::Point3{-1.0, 0.0, -1.0}, 0.4, bubble));
    world.add(std::make_shared<rt::Sphere>(rt::Point3{1.0, 0.0, -1.0}, 0.5, gold));
    return world;
}

}  // namespace

int main(int argc, char* argv[]) {
    // ---- Options ----
    rt::Options opts;
    try {
        opts = rt::parse_options(argc, argv);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << "\n\n" << rt::usage();
        return 2;
    }
    if (opts.show_help) {
        std::cout << rt::usage();
        return 0;
    }

    // ---- Scene and camera ----
    const rt::HittableList world = build_scene();

    rt::CameraSettings settings;  // 16:9, 90 deg FOV
    settings.image_width = opts.width;
    settings.samples_per_pixel = opts.samples;
    settings.max_depth = opts.max_depth;
    const rt::Camera camera{settings};

    std::cout << "Rendering " << camera.image_width() << "x" << camera.image_height()
              << ", " << opts.samples << " samples/pixel, max depth " << opts.max_depth
              << ", " << opts.threads << " thread(s)..." << std::endl;

    // ---- Render (timed) ----
    // steady_clock never jumps (unlike the wall clock), so it's the right
    // clock for measuring durations.
    const auto start = std::chrono::steady_clock::now();
    const rt::Image image = camera.render(world, opts.threads);
    const auto stop = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed = stop - start;  // in seconds

    std::cout << "Render time: " << std::fixed << std::setprecision(3) << elapsed.count()
              << " s\n";

    // ---- Save ----
    std::ofstream file{opts.output};
    if (!file) {
        std::cerr << "Error: could not open '" << opts.output << "' for writing.\n"
                  << "Run from the project root, or pass --output PATH.\n";
        return 1;
    }
    rt::write_ppm(file, image);
    std::cout << "Wrote " << opts.output << "\n";
    return 0;
}
