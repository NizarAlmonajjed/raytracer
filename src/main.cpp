#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "rt/camera.h"
#include "rt/hittable_list.h"
#include "rt/image.h"
#include "rt/sphere.h"

int main(int argc, char* argv[]) {
    const std::string out_path = argc > 1 ? argv[1] : "renders/scene.ppm";

    // ---- Scene ----
    // make_shared allocates the Sphere and its reference count in one go and
    // returns a shared_ptr<Sphere>, which converts to shared_ptr<Hittable>.
    rt::HittableList world;
    world.add(std::make_shared<rt::Sphere>(rt::Point3{0.0, 0.0, -1.0}, 0.5));
    world.add(std::make_shared<rt::Sphere>(rt::Point3{0.0, -100.5, -1.0}, 100.0));  // ground

    // ---- Camera ----
    rt::CameraSettings settings;  // defaults: 400 px wide, 16:9, 90 deg FOV
    settings.samples_per_pixel = 100;
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
              << ", " << settings.samples_per_pixel << " samples/pixel)\n";
    return 0;
}
