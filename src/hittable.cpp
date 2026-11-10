#include "rt/hittable.h"

namespace rt {

void HitRecord::set_face_normal(const Ray& ray, const Vec3& outward_normal) {
    // Opposite directions (negative dot product) => the ray hit the outside.
    front_face = dot(ray.direction(), outward_normal) < 0.0;
    normal = front_face ? outward_normal : -outward_normal;
}

}  // namespace rt
