#pragma once

#include <ostream>
#include <vector>

#include "rt/vec3.h"

namespace rt {

// A width x height grid of linear colours with components in [0, 1].
// Pixel (0, 0) is the top-left corner; rows go downward.
class Image {
public:
    // Throws std::invalid_argument if width or height is not positive.
    Image(int width, int height);

    int width() const { return width_; }
    int height() const { return height_; }

    // Two overloads: the non-const one lets you write image.at(x, y) = colour;
    // the const one lets you read pixels from a const Image.
    Color& at(int x, int y) { return pixels_[index(x, y)]; }
    const Color& at(int x, int y) const { return pixels_[index(x, y)]; }

private:
    int index(int x, int y) const { return y * width_ + x; }

    int width_;
    int height_;
    std::vector<Color> pixels_;
};

// Writes the image in plain-text PPM (P3) format:
//   P3
//   <width> <height>
//   255
//   r g b        <- one line per pixel, row by row from the top
void write_ppm(std::ostream& out, const Image& image);

}  // namespace rt
