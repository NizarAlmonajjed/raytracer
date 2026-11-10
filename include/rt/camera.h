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
    int max_depth{50};              // max bounces per ray before giving up
    double vertical_fov_deg{90.0};  // vertical field of view, in degrees
    Point3 look_from{0.0, 0.0, 0.0};
    Point3 look_at{0.0, 0.0, -1.0};
    Vec3 up{0.0, 1.0, 0.0};         // which way is "up" for the camera
};

class Camera {
public:
    // `explicit` stops the compiler from silently converting a CameraSettings
    // into a Camera, e.g. when passing one to a function expecting a Camera.
    // Throws std::invalid_argument for a non-positive width, sample count or
    // max depth.
    explicit Camera(const CameraSettings& settings);

    int image_width() const { return settings_.image_width; }
    int image_height() const { return image_height_; }

    // The ray from the eye through viewport position (u, v), where (0, 0) is
    // the top-left corner of the image and (1, 1) the bottom-right.
    Ray ray_through(double u, double v) const;

    // Renders the world, averaging samples_per_pixel random rays per pixel.
    // Rows are shared out dynamically between `thread_count` threads (the
    // calling thread counts as one of them). The result is identical for any
    // thread count. Throws std::invalid_argument if thread_count < 1.
    Image render(const Hittable& world, int thread_count = 1) const;

private:
    // Renders one row of pixels into `image`. Safe to call concurrently for
    // DIFFERENT rows: it only reads the world and writes row y of the image.
    void render_row(const Hittable& world, Image& image, int y) const;

    CameraSettings settings_;
    int image_height_;
    Point3 origin_;
    Point3 top_left_;   // top-left corner of the viewport
    Vec3 horizontal_;   // left edge -> right edge of the viewport
    Vec3 vertical_;     // top edge -> bottom edge of the viewport
};

// The (linear) colour of light arriving along `ray`. Follows the ray as it
// bounces off materials, up to `depth` more bounces; returns black when the
// bounce budget runs out or the ray is absorbed, and the sky when it escapes.
Color ray_color(const Ray& ray, const Hittable& world, int depth);

}  // namespace rt
