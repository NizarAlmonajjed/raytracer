#pragma once

#include <string>
#include <vector>

#include "rt/camera.h"
#include "rt/hittable_list.h"

namespace rt {

// A world plus a camera set up to look at it. The caller still chooses the
// image width, samples per pixel and max depth.
struct Scene {
    HittableList world;
    CameraSettings camera;
};

// The scene names accepted by make_scene().
std::vector<std::string> scene_names();

// Builds a named scene:
//   "materials" - 5 spheres: ground, hollow glass, matte and fuzzy metal
//   "random"    - ~490 spheres with random materials: the BVH benchmark scene
// The random scene is generated from a fixed seed, so it is the same every run.
// Throws std::invalid_argument for an unknown name.
Scene make_scene(const std::string& name);

}  // namespace rt
