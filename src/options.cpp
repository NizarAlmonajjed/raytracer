#include "rt/options.h"

#include <stdexcept>
#include <thread>

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
    // TODO(you): fill in the blank, then delete `return 1;` below.
    //
    //   const unsigned int n = std::thread::hardware_concurrency();
    //   return n == 0 ? ___ : static_cast<int>(n);
    //
    // hardware_concurrency() returns the number of hardware threads, but is
    // allowed to return 0 when it can't find out. What should we use then?
    //
    // Tests: tests/options_test.cpp, the "DefaultThreads" tests.
    return 1;
}

Options parse_options(int argc, const char* const argv[]) {
    Options opts;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            opts.show_help = true;
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
    return "Usage: raytracer [options]\n"
           "  --width N     image width in pixels (default " + std::to_string(d.width) + ")\n"
           "  --samples N   rays per pixel (default " + std::to_string(d.samples) + ")\n"
           "  --depth N     max bounces per ray (default " + std::to_string(d.max_depth) + ")\n"
           "  --threads N   worker threads (default " + std::to_string(d.threads) + ")\n"
           "  --output PATH output PPM file (default " + d.output + ")\n"
           "  --help        show this message\n";
}

}  // namespace rt
