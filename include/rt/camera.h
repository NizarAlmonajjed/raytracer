#pragma once

#include "rt/hittable.h"
#include "rt/image.h"
#include "rt/ray.h"
#include "rt/vec3.h"

namespace rt {

// Everything you can configure about the camera. Default member initialisers
// mean you only set what you want to change:
//   CameraSettings s;  s.image_width = 800;
struct CameraSettings {
    double aspect_ratio{16.0 / 9.0};
    int image_width{400};
    int samples_per_pixel{100};     // random rays per pixel (anti-aliasing)
    double vertical_fov_deg{90.0};  // vertical field of view, in degrees
    Point3 look_from{0.0, 0.0, 0.0};
    Point3 look_at{0.0, 0.0, -1.0};
    Vec3 up{0.0, 1.0, 0.0};         // which way is "up" for the camera
};

class Camera {
public:
    // `explicit` stops the compiler from silently converting a CameraSettings
    // into a Camera, e.g. when passing one to a function expecting a Camera.
    // Throws std::invalid_argument for a non-positive width or sample count.
    explicit Camera(const CameraSettings& settings);

    int image_width() const { return settings_.image_width; }
    int image_height() const { return image_height_; }

    // The ray from the eye through viewport position (u, v), where (0, 0) is
    // the top-left corner of the image and (1, 1) the bottom-right.
    Ray ray_through(double u, double v) const;

    // Renders the world, averaging samples_per_pixel random rays per pixel.
    Image render(const Hittable& world) const;

private:
    CameraSettings settings_;
    int image_height_;
    Point3 origin_;
    Point3 top_left_;   // top-left corner of the viewport
    Vec3 horizontal_;   // left edge -> right edge of the viewport
    Vec3 vertical_;     // top edge -> bottom edge of the viewport
};

// The colour seen along `ray`: the hit surface's normal as a colour, or the
// sky gradient if nothing is hit.
Color ray_color(const Ray& ray, const Hittable& world);

}  // namespace rt
