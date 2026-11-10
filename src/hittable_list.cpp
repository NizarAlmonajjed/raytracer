#include "rt/hittable_list.h"

namespace rt {

std::optional<HitRecord> HittableList::hit(const Ray& ray, double t_min,
                                           double t_max) const {
    std::optional<HitRecord> closest;
    double closest_t = t_max;

    for (const auto& object : objects_) {
        // Passing closest_t as the new t_max means only hits NEARER than the
        // best one so far can succeed.
        if (auto rec = object->hit(ray, t_min, closest_t)) {
            closest_t = rec->t;
            closest = rec;
        }
    }
    return closest;
}

}  // namespace rt
