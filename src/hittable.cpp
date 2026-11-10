#include "rt/hittable.h"

namespace rt {

void HitRecord::set_face_normal(const Ray& ray, const Vec3& outward_normal) {
    // TODO(you): fill in the two blanks, then delete the (void) lines.
    //
    //   front_face = dot(__________, __________) < 0.0;
    //   normal = front_face ? outward_normal : -outward_normal;
    //
    // Blanks: the ray's direction, and the outward normal. If they point in
    // opposite directions (negative dot product), the ray hit the outside.
    //
    // Tests: tests/hittable_test.cpp, the "FaceNormal" tests.
    (void)ray;
    (void)outward_normal;
}

}  // namespace rt
