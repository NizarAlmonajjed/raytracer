#pragma once

#include <string>

namespace rt {

// C++17 has no std::numbers::pi (that arrived in C++20), so define our own.
constexpr double kPi = 3.14159265358979323846;

constexpr double degrees_to_radians(double degrees) { return degrees * kPi / 180.0; }

// Converts a linear light value to gamma-encoded (gamma 2), which is what
// image viewers expect. Without it, renders look too dark.
double linear_to_gamma(double linear);

// Returns the greeting printed by the program on startup.
std::string greeting();

// Converts one colour channel from the range [0.0, 1.0] to an integer byte
// in the range [0, 255], which is what the PPM image format expects.
//
// Rules:
//   - Values below 0.0 are treated as 0.0, values above 1.0 as 1.0 (clamping).
//   - Scale by 256 (not 255) and truncate toward zero, so every byte value
//     covers an equal-width slice of [0, 1). To keep 1.0 from becoming 256,
//     clamp the upper end to 0.999 before scaling.
//
// Examples: 0.0 -> 0, 0.5 -> 128, 1.0 -> 255, -0.3 -> 0, 7.0 -> 255
int to_byte(double channel);

}  // namespace rt
