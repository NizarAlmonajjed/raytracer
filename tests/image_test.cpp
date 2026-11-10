#include <sstream>
#include <stdexcept>

#include <gtest/gtest.h>

#include "rt/image.h"
#include "test_helpers.h"

using rt::Color;
using rt::Image;

TEST(Image, NewImageIsBlack) {
    const Image img{3, 2};
    EXPECT_EQ(img.width(), 3);
    EXPECT_EQ(img.height(), 2);
    EXPECT_TRUE(rt_test::VecNear(img.at(2, 1), Color{0, 0, 0}));
}

TEST(Image, RejectsNonPositiveSize) {
    EXPECT_THROW(Image(0, 5), std::invalid_argument);
    EXPECT_THROW(Image(5, -1), std::invalid_argument);
}

TEST(Image, PixelsAreIndependent) {
    Image img{2, 2};
    img.at(1, 0) = Color{1, 0, 0};
    EXPECT_TRUE(rt_test::VecNear(img.at(1, 0), Color{1, 0, 0}));
    EXPECT_TRUE(rt_test::VecNear(img.at(0, 1), Color{0, 0, 0}));
}

TEST(WritePpm, WritesHeaderAndPixelsTopRowFirst) {
    Image img{2, 1};
    img.at(0, 0) = Color{1.0, 0.0, 0.0};  // red
    img.at(1, 0) = Color{0.0, 0.5, 1.0};

    std::ostringstream out;
    rt::write_ppm(out, img);

    EXPECT_EQ(out.str(),
              "P3\n"
              "2 1\n"
              "255\n"
              "255 0 0\n"
              "0 128 255\n");
}
