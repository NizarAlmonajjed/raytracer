#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "rt/camera.h"
#include "rt/hittable_list.h"
#include "rt/image.h"
#include "rt/material.h"
#include "rt/sphere.h"

int main(int argc, char* argv[]) {
    const std::string out_path = argc > 1 ? argv[1] : "renders/materials.ppm";

    // ---- Materials ----
    // Each material is created once and shared by every sphere that uses it.
    const auto ground = std::make_shared<rt::Lambertian>(rt::Color{0.8, 0.8, 0.0});
    const auto matte_blue = std::make_shared<rt::Lambertian>(rt::Color{0.1, 0.2, 0.5});
    const auto glass = std::make_shared<rt::Dielectric>(1.5);
    // An "air bubble" inside the glass: its index is 1/1.5, so light passing
    // from glass into it behaves like glass -> air. Together they make a
    // hollow glass ball.
    const auto bubble = std::make_shared<rt::Dielectric>(1.0 / 1.5);
    const auto gold = std::make_shared<rt::Metal>(rt::Color{0.8, 0.6, 0.2}, 0.3);

    // ---- Scene ----
    rt::HittableList world;
    world.add(std::make_shared<rt::Sphere>(rt::Point3{0.0, -100.5, -1.0}, 100.0, ground));
    world.add(std::make_shared<rt::Sphere>(rt::Point3{0.0, 0.0, -1.2}, 0.5, matte_blue));
    world.add(std::make_shared<rt::Sphere>(rt::Point3{-1.0, 0.0, -1.0}, 0.5, glass));
    world.add(std::make_shared<rt::Sphere>(rt::Point3{-1.0, 0.0, -1.0}, 0.4, bubble));
    world.add(std::make_shared<rt::Sphere>(rt::Point3{1.0, 0.0, -1.0}, 0.5, gold));

    // ---- Camera ----
    rt::CameraSettings settings;  // 400 px wide, 16:9, 90 deg FOV
    settings.samples_per_pixel = 100;
    settings.max_depth = 50;
    const rt::Camera camera{settings};

    // ---- Render ----
    const rt::Image image = camera.render(world);

    // ---- Save ----
    std::ofstream file{out_path};
    if (!file) {
        std::cerr << "Error: could not open '" << out_path << "' for writing.\n"
                  << "Run from the project root, or pass an output path.\n";
        return 1;
    }
    rt::write_ppm(file, image);
    std::cout << "Wrote " << out_path << " (" << image.width() << "x" << image.height()
              << ", " << settings.samples_per_pixel << " samples/pixel, max depth "
              << settings.max_depth << ")\n";
    return 0;
}
