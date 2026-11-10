#include "rt/camera.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "rt/material.h"
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
    if (settings.max_depth <= 0) {
        throw std::invalid_argument("max_depth must be positive");
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
                sum += ray_color(ray, world, settings_.max_depth);
            }
            image.at(x, y) = sum / static_cast<double>(samples);
        }
    }
    return image;  // returned by move (or constructed in place), not copied
}

Color ray_color(const Ray& ray, const Hittable& world, int depth) {
    constexpr double kInfinity = std::numeric_limits<double>::infinity();
    // Ignore hits closer than this, so a bounced ray doesn't immediately
    // re-hit the surface it left because of rounding error ("shadow acne").
    constexpr double kTMin = 0.001;

    if (depth <= 0) {
        return Color{0.0, 0.0, 0.0};  // bounce budget exhausted: no light
    }

    // world.hit() is a virtual call: it runs HittableList::hit, Sphere::hit,
    // or whatever the real type is.
    if (const auto rec = world.hit(ray, kTMin, kInfinity)) {
        if (rec->material == nullptr) {
            return Color{0.0, 0.0, 0.0};
        }
        // Another virtual call: Lambertian, Metal or Dielectric decides.
        if (const auto scatter = rec->material->scatter(ray, *rec)) {
            // Recursion: the light we see is the light the bounced ray sees,
            // tinted by the surface.
            return scatter->attenuation * ray_color(scatter->scattered, world, depth - 1);
        }
        return Color{0.0, 0.0, 0.0};  // absorbed
    }

    // Missed everything: sky gradient, white at the bottom to blue at the top.
    // The sky is our only light source.
    const Vec3 dir = unit_vector(ray.direction());
    const double a = 0.5 * (dir.y + 1.0);
    return (1.0 - a) * Color{1.0, 1.0, 1.0} + a * Color{0.5, 0.7, 1.0};
}

}  // namespace rt
