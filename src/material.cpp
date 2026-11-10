#include "rt/material.h"

#include <algorithm>
#include <cmath>

#include "rt/random.h"

namespace rt {

// ---- Lambertian -------------------------------------------------------------

std::optional<ScatterResult> Lambertian::scatter(const Ray& /*ray_in*/,
                                                 const HitRecord& rec) const {
    // A random point on a unit sphere sitting on the surface: directions near
    // the normal are more likely than grazing ones, like real matte surfaces.
    Vec3 direction = rec.normal + random_unit_vector();

    // If the random vector almost exactly cancels the normal, the direction
    // is ~zero and would produce NaNs later. Fall back to the normal.
    if (direction.near_zero()) {
        direction = rec.normal;
    }
    return ScatterResult{Ray{rec.point, direction}, albedo_};
}

// ---- Metal ------------------------------------------------------------------

Metal::Metal(const Color& albedo, double fuzz)
    : albedo_{albedo}, fuzz_{std::clamp(fuzz, 0.0, 1.0)} {}

std::optional<ScatterResult> Metal::scatter(const Ray& ray_in,
                                            const HitRecord& rec) const {
    const Vec3 reflected = unit_vector(reflect(ray_in.direction(), rec.normal));
    const Vec3 direction = reflected + fuzz_ * random_unit_vector();

    // Fuzz can push the ray below the surface; treat that as absorbed.
    if (dot(direction, rec.normal) <= 0.0) {
        return std::nullopt;
    }
    return ScatterResult{Ray{rec.point, direction}, albedo_};
}

// ---- Dielectric -------------------------------------------------------------

std::optional<ScatterResult> Dielectric::scatter(const Ray& ray_in,
                                                 const HitRecord& rec) const {
    // Entering the material: air (1.0) -> glass. Leaving: glass -> air.
    const double eta_ratio = rec.front_face ? 1.0 / refraction_index_ : refraction_index_;

    const Vec3 unit_dir = unit_vector(ray_in.direction());
    const double cos_theta = std::fmin(dot(-unit_dir, rec.normal), 1.0);
    const double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    // Snell's law has no solution if this is > 1: total internal reflection.
    const bool cannot_refract = eta_ratio * sin_theta > 1.0;

    Vec3 direction;
    if (cannot_refract || reflectance(cos_theta, eta_ratio) > random_double()) {
        direction = reflect(unit_dir, rec.normal);
    } else {
        direction = refract(unit_dir, rec.normal, eta_ratio);
    }

    // Clear glass absorbs nothing.
    return ScatterResult{Ray{rec.point, direction}, Color{1.0, 1.0, 1.0}};
}

double Dielectric::reflectance(double cosine, double eta_ratio) {
    double r0 = (1.0 - eta_ratio) / (1.0 + eta_ratio);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * std::pow(1.0 - cosine, 5.0);
}

}  // namespace rt
