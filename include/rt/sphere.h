#pragma once

#include <optional>

#include "rt/ray.h"
#include "rt/vec3.h"

namespace rt {

// A plain aggregate: just data, no invariants enforced yet.
// (In milestone 3 this becomes a class that derives from Hittable.)
struct Sphere {
    Point3 center;
    double radius{1.0};
};

// Returns the smallest t in the open interval (t_min, t_max) at which `ray`
// hits `sphere`, or std::nullopt if it doesn't hit within that interval.
//
// Solves |O + t*d - C|^2 = r^2, i.e. the quadratic
//   (d.d) t^2 - 2 (d.oc) t + (oc.oc - r^2) = 0,   where oc = C - O.
std::optional<double> intersect(const Sphere& sphere, const Ray& ray,
                                double t_min, double t_max);

// Returns the outward-pointing unit normal at `point`, which must lie on the
// sphere's surface. The normal points from the centre to the point, and has
// length 1 because |point - center| == radius.
Vec3 sphere_normal(const Sphere& sphere, const Point3& point);

}  // namespace rt
