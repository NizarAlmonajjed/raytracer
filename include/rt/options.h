#pragma once

#include <string>

namespace rt {

// How many threads to use by default: one per hardware thread, or 1 if the
// system can't tell us.
int default_thread_count();

// Command-line settings for the renderer.
struct Options {
    int width{400};
    int samples{100};
    int max_depth{50};
    int threads{default_thread_count()};
    std::string output{"renders/materials.ppm"};
    bool show_help{false};
};

// Parses arguments like:  --width 800 --samples 200 --threads 8
// argv[0] (the program name) is skipped.
// Throws std::invalid_argument with a readable message on bad input.
Options parse_options(int argc, const char* const argv[]);

// The text printed for --help.
std::string usage();

}  // namespace rt
