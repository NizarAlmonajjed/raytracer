#include "rt/bvh.h"

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace rt {

BvhNode::BvhNode(std::shared_ptr<Hittable> left, std::shared_ptr<Hittable> right)
    : left_{std::move(left)}, right_{std::move(right)} {
    box_ = AABB::surrounding(left_->bounding_box(), right_->bounding_box());
}

std::optional<HitRecord> BvhNode::hit(const Ray& ray, double t_min, double t_max) const {
    // The whole point of the BVH: one cheap box test can rule out every
    // object below this node.
    if (!box_.hit(ray, t_min, t_max)) {
        return std::nullopt;
    }
    const auto left_hit = left_->hit(ray, t_min, t_max);
    // If the left child was hit, only a CLOSER hit on the right can matter.
    const auto right_hit = right_->hit(ray, t_min, left_hit ? left_hit->t : t_max);
    return right_hit ? right_hit : left_hit;
}

namespace {

using Objects = std::vector<std::shared_ptr<Hittable>>;

// Builds a subtree over objects[begin, end). Reorders that range in place.
std::shared_ptr<Hittable> build_range(Objects& objects, std::size_t begin, std::size_t end) {
    const std::size_t count = end - begin;
    if (count == 1) {
        return objects[begin];  // a leaf is just the object itself
    }
    if (count == 2) {
        return std::make_shared<BvhNode>(objects[begin], objects[begin + 1]);
    }

    // Split along the axis where the objects are most spread out.
    AABB bounds;
    for (std::size_t i = begin; i < end; ++i) {
        bounds = AABB::surrounding(bounds, objects[i]->bounding_box());
    }
    const int axis = bounds.longest_axis();

    // Put the "lower" half of the objects (by box centre) first. nth_element
    // only partially sorts: everything before `mid` is <= everything after.
    // That's all we need, and it's O(n) instead of std::sort's O(n log n).
    const std::size_t mid = begin + count / 2;
    const auto by_centroid = [axis](const std::shared_ptr<Hittable>& a,
                                    const std::shared_ptr<Hittable>& b) {
        return a->bounding_box().centroid()[axis] < b->bounding_box().centroid()[axis];
    };
    // Iterator arithmetic needs a signed type.
    const auto first = objects.begin() + static_cast<std::ptrdiff_t>(begin);
    std::nth_element(first, objects.begin() + static_cast<std::ptrdiff_t>(mid),
                     objects.begin() + static_cast<std::ptrdiff_t>(end), by_centroid);

    return std::make_shared<BvhNode>(build_range(objects, begin, mid),
                                     build_range(objects, mid, end));
}

}  // namespace

std::shared_ptr<Hittable> build_bvh(std::vector<std::shared_ptr<Hittable>> objects) {
    if (objects.empty()) {
        throw std::invalid_argument("cannot build a BVH with no objects");
    }
    return build_range(objects, 0, objects.size());
}

}  // namespace rt
