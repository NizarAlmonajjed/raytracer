#include <fstream>
#include <iostream>
#include <string>

#include "rt/image.h"
#include "rt/ray.h"
#include "rt/vec3.h"

namespace {

// The colour a ray "sees". With no objects in the scene yet, every ray sees
// sky: blend from white at the bottom to light blue at the top, based on how
// steeply the ray points up.
rt::Color ray_color(const rt::Ray& ray) {
    const rt::Vec3 dir = rt::unit_vector(ray.direction());
    const double a = 0.5 * (dir.y + 1.0);  // map y from [-1, 1] to [0, 1]
    const rt::Color white{1.0, 1.0, 1.0};
    const rt::Color sky_blue{0.5, 0.7, 1.0};
    return (1.0 - a) * white + a * sky_blue;  // linear interpolation (lerp)
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::string out_path = argc > 1 ? argv[1] : "renders/gradient.ppm";

    // ---- Image ----
    const double aspect_ratio = 16.0 / 9.0;
    const int width = 400;
    const int height = static_cast<int>(width / aspect_ratio);
    rt::Image image{width, height};

    // ---- Camera ----
    // The eye sits at the origin looking down -z. One unit in front of it is a
    // virtual "window" (the viewport); each pixel is a small square on it.
    const rt::Point3 eye{0.0, 0.0, 0.0};
    const double focal_length = 1.0;
    const double viewport_height = 2.0;
    const double viewport_width = viewport_height * (static_cast<double>(width) / height);

    const rt::Vec3 horizontal{viewport_width, 0.0, 0.0};   // left edge -> right edge
    const rt::Vec3 vertical{0.0, -viewport_height, 0.0};   // top edge -> bottom edge
    const rt::Point3 top_left =
        eye - rt::Vec3{0.0, 0.0, focal_length} - horizontal / 2.0 - vertical / 2.0;

    // ---- Render ----
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // +0.5 aims at the centre of the pixel, not its corner.
            const double u = (x + 0.5) / width;
            const double v = (y + 0.5) / height;
            const rt::Point3 pixel = top_left + u * horizontal + v * vertical;
            const rt::Ray ray{eye, pixel - eye};
            image.at(x, y) = ray_color(ray);
        }
    }

    // ---- Save ----
    std::ofstream file{out_path};
    if (!file) {
        std::cerr << "Error: could not open '" << out_path << "' for writing.\n"
                  << "Run from the project root, or pass an output path.\n";
        return 1;
    }
    rt::write_ppm(file, image);
    std::cout << "Wrote " << out_path << " (" << width << "x" << height << ")\n";
    return 0;
}  // `file` is closed automatically here (RAII)
