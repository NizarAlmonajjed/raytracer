#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "rt/bvh.h"
#include "rt/camera.h"
#include "rt/image.h"
#include "rt/options.h"
#include "rt/scenes.h"

namespace {

// Seconds elapsed since `start`, measured with a clock that never jumps.
double seconds_since(std::chrono::steady_clock::time_point start) {
    const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - start;
    return elapsed.count();
}

}  // namespace

int main(int argc, char* argv[]) {
    // ---- Options ----
    rt::Options opts;
    try {
        opts = rt::parse_options(argc, argv);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << "\n\n" << rt::usage();
        return 2;
    }
    if (opts.show_help) {
        std::cout << rt::usage();
        return 0;
    }

    // ---- Scene and camera ----
    rt::Scene scene = rt::make_scene(opts.scene);
    scene.camera.image_width = opts.width;
    scene.camera.samples_per_pixel = opts.samples;
    scene.camera.max_depth = opts.max_depth;
    const rt::Camera camera{scene.camera};

    // ---- Acceleration structure ----
    // `world` refers either to the BVH or to the plain list. Both are
    // Hittables, so the renderer doesn't know or care which it gets.
    std::cout << std::fixed << std::setprecision(3);
    std::shared_ptr<rt::Hittable> bvh;
    if (opts.use_bvh) {
        const auto start = std::chrono::steady_clock::now();
        bvh = rt::build_bvh(scene.world.objects());
        std::cout << "Built BVH over " << scene.world.size() << " objects in "
                  << seconds_since(start) << " s\n";
    }
    const rt::Hittable& world = opts.use_bvh ? *bvh : scene.world;

    std::cout << "Rendering scene '" << opts.scene << "' (" << scene.world.size()
              << " objects, " << (opts.use_bvh ? "BVH" : "no BVH") << "), "
              << camera.image_width() << "x" << camera.image_height() << ", "
              << opts.samples << " samples/pixel, max depth " << opts.max_depth << ", "
              << opts.threads << " thread(s)..." << std::endl;

    // ---- Render (timed) ----
    const auto start = std::chrono::steady_clock::now();
    const rt::Image image = camera.render(world, opts.threads);
    std::cout << "Render time: " << seconds_since(start) << " s\n";

    // ---- Save ----
    const std::string out_path = opts.output_path();
    std::ofstream file{out_path};
    if (!file) {
        std::cerr << "Error: could not open '" << out_path << "' for writing.\n"
                  << "Run from the project root, or pass --output PATH.\n";
        return 1;
    }
    rt::write_ppm(file, image);
    std::cout << "Wrote " << out_path << "\n";
    return 0;
}
