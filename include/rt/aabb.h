#pragma once

#include <algorithm>
#include <limits>

#include "rt/ray.h"
#include "rt/vec3.h"

namespace rt {

// Axis-aligned bounding box: all points p with min <= p <= max on every axis.
// The default box is EMPTY (min = +inf, max = -inf), so surrounding it with
// any other box just gives that other box.
class AABB {
public:
    AABB() = default;
    AABB(const Point3& min, const Point3& max) : min_{min}, max_{max} {}

    const Point3& min() const { return min_; }
    const Point3& max() const { return max_; }

    bool is_empty() const { return min_.x > max_.x || min_.y > max_.y || min_.z > max_.z; }

    Point3 centroid() const { return 0.5 * (min_ + max_); }

    // 0 for x, 1 for y, 2 for z: whichever side of the box is longest.
    int longest_axis() const {
        const Vec3 size = max_ - min_;
        if (size.x >= size.y && size.x >= size.z) {
            return 0;
        }
        return size.y >= size.z ? 1 : 2;
    }

    // Slab test. On each axis the ray is between the box's two planes for
    // t in [t0, t1]; the ray hits the box if all three intervals (and
    // (t_min, t_max)) overlap.
    bool hit(const Ray& ray, double t_min, double t_max) const {
        for (int axis = 0; axis < 3; ++axis) {
            // Dividing by a zero direction gives +/-infinity, which is exactly
            // right: the ray never crosses that slab's planes.
            const double inv_d = 1.0 / ray.direction()[axis];
            double t0 = (min_[axis] - ray.origin()[axis]) * inv_d;
            double t1 = (max_[axis] - ray.origin()[axis]) * inv_d;
            if (inv_d < 0.0) {
                std::swap(t0, t1);  // ray goes in the negative direction
            }
            // Written with if-statements (not std::max/min) so that a NaN
            // (0 * infinity, a ray lying exactly on a plane) is ignored.
            if (t0 > t_min) {
                t_min = t0;
            }
            if (t1 < t_max) {
                t_max = t1;
            }
            if (t_max <= t_min) {
                return false;
            }
        }
        return true;
    }

    // The smallest box containing both a and b.
    static AABB surrounding(const AABB& a, const AABB& b) {
        return AABB{Point3{std::min(a.min_.x, b.min_.x), std::min(a.min_.y, b.min_.y),
                           std::min(a.min_.z, b.min_.z)},
                    Point3{std::max(a.max_.x, b.max_.x), std::max(a.max_.y, b.max_.y),
                           std::max(a.max_.z, b.max_.z)}};
    }

private:
    static constexpr double kInf = std::numeric_limits<double>::infinity();
    Point3 min_{kInf, kInf, kInf};
    Point3 max_{-kInf, -kInf, -kInf};
};

}  // namespace rt
