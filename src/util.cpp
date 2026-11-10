#include "rt/util.h"
#include <algorithm>

namespace rt {

std::string greeting() {
    return "Hello, ray tracer!";
}

int to_byte(double channel) {
    double c = std::clamp(channel, 0.0, 0.999);
    return static_cast<int>(c * 256);
}

}  // namespace rt
