#pragma once

#include <optional>

#include "rt/hittable.h"
#include "rt/ray.h"
#include "rt/vec3.h"

namespace rt {

// What happens when light hits a surface: the ray continues as `scattered`,
// and each colour channel is multiplied by `attenuation` (how much survives).
struct ScatterResult {
    Ray scattered;
    Color attenuation;
};

// Abstract base class for surface materials. Returns std::nullopt if the ray
// is absorbed.
class Material {
public:
    virtual ~Material() = default;
    virtual std::optional<ScatterResult> scatter(const Ray& ray_in,
                                                 const HitRecord& rec) const = 0;
};

// Matte surface: scatters in a random direction biased toward the normal.
// `final` means nothing can derive from this class (and lets the compiler
// skip the virtual lookup when it knows the exact type).
class Lambertian final : public Material {
public:
    explicit Lambertian(const Color& albedo) : albedo_{albedo} {}

    std::optional<ScatterResult> scatter(const Ray& ray_in,
                                         const HitRecord& rec) const override;

private:
    Color albedo_;  // fraction of each colour channel the surface reflects
};

// Mirror-like surface. fuzz in [0, 1]: 0 is a perfect mirror, 1 is very blurry.
class Metal final : public Material {
public:
    Metal(const Color& albedo, double fuzz);

    std::optional<ScatterResult> scatter(const Ray& ray_in,
                                         const HitRecord& rec) const override;

private:
    Color albedo_;
    double fuzz_;
};

// Clear material such as glass (1.5) or water (1.33) that refracts light.
class Dielectric final : public Material {
public:
    explicit Dielectric(double refraction_index) : refraction_index_{refraction_index} {}

    std::optional<ScatterResult> scatter(const Ray& ray_in,
                                         const HitRecord& rec) const override;

    // Schlick's approximation: probability that light is reflected rather
    // than refracted, given the cosine of the incoming angle. `static` means
    // it doesn't need an object: call it as Dielectric::reflectance(...).
    static double reflectance(double cosine, double eta_ratio);

private:
    double refraction_index_;
};

}  // namespace rt
