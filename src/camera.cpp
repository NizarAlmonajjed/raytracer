#include "rt/camera.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "rt/random.h"
#include "rt/util.h"

namespace rt {

Camera::Camera(const CameraSettings& settings) : settings_{settings} {
    if (settings.image_width <= 0) {
        throw std::invalid_argument("image_width must be positive");
    }
    if (settings.samples_per_pixel <= 0) {
        throw std::invalid_argument("samples_per_pixel must be positive");
    }
    image_height_ = std::max(1, static_cast<int>(settings.image_width / settings.aspect_ratio));

    // Viewport size, one unit in front of the eye. tan(fov/2) is half the
    // viewport height at distance 1.
    const double theta = degrees_to_radians(settings.vertical_fov_deg);
    const double viewport_height = 2.0 * std::tan(theta / 2.0);
    // Use the real pixel ratio (after integer rounding of the height).
    const double viewport_width =
        viewport_height * (static_cast<double>(settings.image_width) / image_height_);

    // Orthonormal camera basis, built with cross products:
    //   w points backward (from target to eye), u points right, v points up.
    const Vec3 w = unit_vector(settings.look_from - settings.look_at);
    const Vec3 u = unit_vector(cross(settings.up, w));
    const Vec3 v = cross(w, u);

    origin_ = settings.look_from;
    horizontal_ = viewport_width * u;
    vertical_ = -viewport_height * v;  // image rows go downward
    top_left_ = origin_ - w - horizontal_ / 2.0 - vertical_ / 2.0;
}

Ray Camera::ray_through(double u, double v) const {
    const Point3 target = top_left_ + u * horizontal_ + v * vertical_;
    return Ray{origin_, target - origin_};
}

Image Camera::render(const Hittable& world) const {
    const int width = settings_.image_width;
    const int height = image_height_;
    const int samples = settings_.samples_per_pixel;
    Image image{width, height};

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Color sum{0.0, 0.0, 0.0};
            for (int s = 0; s < samples; ++s) {
                // With one sample, aim at the pixel centre; otherwise pick a
                // random point inside the pixel.
                const double dx = samples == 1 ? 0.5 : random_double();
                const double dy = samples == 1 ? 0.5 : random_double();
                const Ray ray = ray_through((x + dx) / width, (y + dy) / height);
                sum += ray_color(ray, world);
            }
            image.at(x, y) = sum / static_cast<double>(samples);
        }
    }
    return image;  // returned by move (or constructed in place), not copied
}

Color ray_color(const Ray& ray, const Hittable& world) {
    constexpr double kInfinity = std::numeric_limits<double>::infinity();

    // world.hit() is a virtual call: it runs HittableList::hit, Sphere::hit,
    // or whatever the real type is.
    if (const auto rec = world.hit(ray, 0.0, kInfinity)) {
        // Map each normal component from [-1, 1] to a colour in [0, 1].
        return 0.5 * (rec->normal + Color{1.0, 1.0, 1.0});
    }

    // Missed everything: sky gradient, white at the bottom to blue at the top.
    const Vec3 dir = unit_vector(ray.direction());
    const double a = 0.5 * (dir.y + 1.0);
    return (1.0 - a) * Color{1.0, 1.0, 1.0} + a * Color{0.5, 0.7, 1.0};
}

}  // namespace rt
