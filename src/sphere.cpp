#include "rt/sphere.h"

#include <cmath>

namespace rt {

std::optional<double> intersect(const Sphere& sphere, const Ray& ray,
                                double t_min, double t_max) {
    // Coefficients of a t^2 - 2h t + c = 0 (the "half-b" form: h = -b/2).
    const Vec3 oc = sphere.center - ray.origin();
    const double a = ray.direction().length_squared();
    const double h = dot(ray.direction(), oc);
    const double c = oc.length_squared() - sphere.radius * sphere.radius;

    const double discriminant = h * h - a * c;
    if (discriminant < 0.0) {
        return std::nullopt;  // the ray misses the sphere entirely
    }

    // Try the nearer root first; if it's outside the allowed range (e.g. behind
    // the ray's origin), try the farther one.
    const double sqrt_d = std::sqrt(discriminant);
    const auto in_range = [&](double t) { return t > t_min && t < t_max; };

    if (const double near = (h - sqrt_d) / a; in_range(near)) {
        return near;
    }
    if (const double far = (h + sqrt_d) / a; in_range(far)) {
        return far;
    }
    return std::nullopt;
}

Vec3 sphere_normal(const Sphere& sphere, const Point3& point) {
    return (point - sphere.center) / sphere.radius;
}

}  // namespace rt
