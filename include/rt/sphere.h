#pragma once

#include <optional>

#include "rt/hittable.h"
#include "rt/ray.h"
#include "rt/vec3.h"

namespace rt {

// A sphere is one kind of Hittable. `public Hittable` means: Sphere IS-A
// Hittable, and can be used anywhere a Hittable is expected.
class Sphere : public Hittable {
public:
    // Throws std::invalid_argument if radius is not positive.
    Sphere(const Point3& center, double radius);

    const Point3& center() const { return center_; }
    double radius() const { return radius_; }

    // `override` asks the compiler to check that this really overrides a
    // virtual function in the base class (catches typos and signature drift).
    std::optional<HitRecord> hit(const Ray& ray, double t_min,
                                 double t_max) const override;

private:
    Point3 center_;
    double radius_;
};

// Returns the smallest t in the open interval (t_min, t_max) at which `ray`
// hits `sphere`, or std::nullopt if it doesn't hit within that interval.
//
// Solves |O + t*d - C|^2 = r^2, i.e. the quadratic
//   (d.d) t^2 - 2 (d.oc) t + (oc.oc - r^2) = 0,   where oc = C - O.
std::optional<double> intersect(const Sphere& sphere, const Ray& ray,
                                double t_min, double t_max);

// Returns the outward-pointing unit normal at `point`, which must lie on the
// sphere's surface.
Vec3 sphere_normal(const Sphere& sphere, const Point3& point);

}  // namespace rt
