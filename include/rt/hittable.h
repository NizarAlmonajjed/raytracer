#pragma once

#include <optional>

#include "rt/ray.h"
#include "rt/vec3.h"

namespace rt {

// Forward declaration: tells the compiler "Material is a class" without
// including its header. Enough to declare a pointer to it, and it avoids a
// circular include (material.h needs HitRecord from this file).
class Material;

// Everything we need to know about where a ray hit a surface.
struct HitRecord {
    Point3 point;           // where the ray hit
    Vec3 normal;            // unit normal, always pointing AGAINST the ray
    double t{0.0};          // ray parameter: point == ray.at(t)
    bool front_face{true};  // true if the ray hit the outside of the surface

    // The surface's material. A raw pointer here means "non-owning": the
    // object that was hit owns its material (via shared_ptr) and outlives this
    // record. Null means the surface absorbs all light.
    const Material* material{nullptr};

    // Given the surface's OUTWARD unit normal, decides which side the ray hit
    // and stores a normal that points against the ray.
    void set_face_normal(const Ray& ray, const Vec3& outward_normal);
};

// Abstract base class (an "interface"): anything a ray can hit.
//
// `virtual` means calls are dispatched at runtime to the derived class's
// version, even through a Hittable& or Hittable*. `= 0` makes the function
// pure virtual: Hittable can't be instantiated, and every concrete subclass
// must implement hit().
class Hittable {
public:
    // A base class used polymorphically needs a virtual destructor, so that
    // deleting a Sphere through a Hittable pointer runs Sphere's destructor.
    virtual ~Hittable() = default;

    // Returns the closest hit with t in (t_min, t_max), or std::nullopt.
    virtual std::optional<HitRecord> hit(const Ray& ray, double t_min,
                                         double t_max) const = 0;
};

}  // namespace rt
