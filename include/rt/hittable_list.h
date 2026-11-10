#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

#include "rt/hittable.h"

namespace rt {

// A scene: a collection of objects that is itself Hittable, so code that
// renders "a Hittable" works the same for one sphere or a whole world.
class HittableList : public Hittable {
public:
    // Takes shared ownership of the object. Pass the shared_ptr by value and
    // std::move it into the vector: one copy at most, often zero.
    void add(std::shared_ptr<Hittable> object) {
        box_ = AABB::surrounding(box_, object->bounding_box());
        objects_.push_back(std::move(object));
    }
    void clear() {
        objects_.clear();
        box_ = AABB{};
    }
    std::size_t size() const { return objects_.size(); }
    const std::vector<std::shared_ptr<Hittable>>& objects() const { return objects_; }

    // Asks every object and returns the closest hit.
    std::optional<HitRecord> hit(const Ray& ray, double t_min,
                                 double t_max) const override;

    AABB bounding_box() const override { return box_; }

private:
    // shared_ptr: reference-counted ownership. The object is deleted
    // automatically when the last shared_ptr to it goes away.
    std::vector<std::shared_ptr<Hittable>> objects_;
    AABB box_;  // grows as objects are added; empty for an empty list
};

}  // namespace rt
