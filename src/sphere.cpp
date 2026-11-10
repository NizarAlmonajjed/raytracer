#include "rt/sphere.h"

#include <cmath>
#include <stdexcept>

namespace rt {

Sphere::Sphere(const Point3& center, double radius, std::shared_ptr<Material> material)
    : center_{center}, radius_{radius}, material_{std::move(material)} {
    if (radius <= 0.0) {
        throw std::invalid_argument("Sphere radius must be positive");
    }
}

std::optional<HitRecord> Sphere::hit(const Ray& ray, double t_min,
                                     double t_max) const {
    const auto t = intersect(*this, ray, t_min, t_max);
    if (!t) {
        return std::nullopt;
    }
    HitRecord rec;
    rec.t = *t;
    rec.point = ray.at(*t);
    rec.set_face_normal(ray, sphere_normal(*this, rec.point));
    rec.material = material_.get();  // non-owning pointer to our material
    return rec;
}

AABB Sphere::bounding_box() const {
    const Vec3 r{radius_, radius_, radius_};
    return AABB{center_ - r, center_ + r};
}

std::optional<double> intersect(const Sphere& sphere, const Ray& ray,
                                double t_min, double t_max) {
    // Coefficients of a t^2 - 2h t + c = 0 (the "half-b" form: h = -b/2).
    const Vec3 oc = sphere.center() - ray.origin();
    const double a = ray.direction().length_squared();
    const double h = dot(ray.direction(), oc);
    const double c = oc.length_squared() - sphere.radius() * sphere.radius();

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
    return (point - sphere.center()) / sphere.radius();
}

}  // namespace rt
