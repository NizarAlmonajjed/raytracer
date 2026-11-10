#include "rt/image.h"

#include <stdexcept>

#include "rt/util.h"

namespace rt {

Image::Image(int width, int height)
    : width_{width}, height_{height} {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Image dimensions must be positive");
    }
    // Allocate after validating, so a negative size never reaches the vector.
    pixels_.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
}

void write_ppm(std::ostream& out, const Image& image) {
    out << "P3\n" << image.width() << ' ' << image.height() << "\n255\n";
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            const Color& c = image.at(x, y);
            out << to_byte(linear_to_gamma(c.x)) << ' '
                << to_byte(linear_to_gamma(c.y)) << ' '
                << to_byte(linear_to_gamma(c.z)) << '\n';
        }
    }
}

}  // namespace rt
