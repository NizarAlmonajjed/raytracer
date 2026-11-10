#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "rt/aabb.h"
#include "rt/hittable.h"

namespace rt {

// One node of a bounding volume hierarchy: a box around two children, each
// of which is either another BvhNode or an actual object (e.g. a Sphere).
// If a ray misses the box, everything inside is skipped with one test.
class BvhNode final : public Hittable {
public:
    BvhNode(std::shared_ptr<Hittable> left, std::shared_ptr<Hittable> right);

    std::optional<HitRecord> hit(const Ray& ray, double t_min,
                                 double t_max) const override;

    AABB bounding_box() const override { return box_; }

private:
    std::shared_ptr<Hittable> left_;
    std::shared_ptr<Hittable> right_;
    AABB box_;
};

// Builds a BVH over `objects` and returns its root. With one object, the
// root is that object itself. The vector is taken by value because the build
// reorders it. Throws std::invalid_argument if `objects` is empty.
std::shared_ptr<Hittable> build_bvh(std::vector<std::shared_ptr<Hittable>> objects);

}  // namespace rt
