#pragma once

#include "rt/vec3.h"

namespace rt {

// A half-line: P(t) = origin + t * direction, for t >= 0.
class Ray {
public:
    Ray() = default;
    Ray(const Point3& origin, const Vec3& direction)
        : origin_{origin}, direction_{direction} {}

    const Point3& origin() const { return origin_; }
    const Vec3& direction() const { return direction_; }

    // The point reached after travelling "t" units of direction from the origin.
    Point3 at(double t) const { return origin_ + t * direction_; }

private:
    Point3 origin_;
    Vec3 direction_;
};

}  // namespace rt
