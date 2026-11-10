#include "rt/options.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <thread>

#include "rt/scenes.h"

namespace rt {

namespace {

// Converts `text` to a positive int, or throws with a message naming the flag.
int parse_positive_int(const std::string& flag, const std::string& text) {
    std::size_t consumed = 0;
    int value = 0;
    try {
        value = std::stoi(text, &consumed);
    } catch (const std::logic_error&) {
        // std::stoi throws invalid_argument ("abc") or out_of_range ("99999999999").
        consumed = 0;
    }
    // Reject partial parses like "12abc", as well as zero and negatives.
    if (consumed == 0 || consumed != text.size() || value <= 0) {
        throw std::invalid_argument(flag + " expects a positive whole number, got '" +
                                    text + "'");
    }
    return value;
}

}  // namespace

int default_thread_count() {
    // hardware_concurrency() may return 0 if it can't tell; 1 always works.
    const unsigned int n = std::thread::hardware_concurrency();
    return n == 0 ? 1 : static_cast<int>(n);
}

Options parse_options(int argc, const char* const argv[]) {
    Options opts;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        // Flags without a value.
        if (arg == "--help" || arg == "-h") {
            opts.show_help = true;
            continue;
        }
        if (arg == "--no-bvh") {
            opts.use_bvh = false;
            continue;
        }

        // Every other option takes a value: the next argument.
        if (i + 1 >= argc) {
            throw std::invalid_argument("missing value after " + arg);
        }
        const std::string value = argv[++i];

        if (arg == "--width") {
            opts.width = parse_positive_int(arg, value);
        } else if (arg == "--samples") {
            opts.samples = parse_positive_int(arg, value);
        } else if (arg == "--depth") {
            opts.max_depth = parse_positive_int(arg, value);
        } else if (arg == "--threads") {
            opts.threads = parse_positive_int(arg, value);
        } else if (arg == "--scene") {
            const auto names = scene_names();
            if (std::find(names.begin(), names.end(), value) == names.end()) {
                throw std::invalid_argument("unknown scene '" + value + "'");
            }
            opts.scene = value;
        } else if (arg == "--output") {
            opts.output = value;
        } else {
            throw std::invalid_argument("unknown option " + arg);
        }
    }
    return opts;
}

std::string usage() {
    const Options d;  // defaults
    std::string scenes;
    for (const auto& name : scene_names()) {
        scenes += (scenes.empty() ? "" : ", ") + name;
    }
    return "Usage: raytracer [options]\n"
           "  --scene NAME  scene to render: " + scenes + " (default " + d.scene + ")\n"
           "  --width N     image width in pixels (default " + std::to_string(d.width) + ")\n"
           "  --samples N   rays per pixel (default " + std::to_string(d.samples) + ")\n"
           "  --depth N     max bounces per ray (default " + std::to_string(d.max_depth) + ")\n"
           "  --threads N   worker threads (default " + std::to_string(d.threads) + ")\n"
           "  --no-bvh      test every object for every ray (for benchmarking)\n"
           "  --output PATH output PPM file (default renders/<scene>.ppm)\n"
           "  --help        show this message\n";
}

}  // namespace rt
