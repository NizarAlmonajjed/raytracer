#include <gtest/gtest.h>

#include "rt/util.h"

TEST(Greeting, ReturnsExpectedText) {
    EXPECT_EQ(rt::greeting(), "Hello, ray tracer!");
}

// ---- Tests for your TODO: rt::to_byte ---------------------------------------

TEST(ToByte, ZeroMapsToZero) {
    EXPECT_EQ(rt::to_byte(0.0), 0);
}

TEST(ToByte, OneMapsTo255) {
    EXPECT_EQ(rt::to_byte(1.0), 255);
}

TEST(ToByte, HalfMapsTo128) {
    EXPECT_EQ(rt::to_byte(0.5), 128);
}

TEST(ToByte, NegativeIsClampedToZero) {
    EXPECT_EQ(rt::to_byte(-0.3), 0);
}

TEST(ToByte, AboveOneIsClampedTo255) {
    EXPECT_EQ(rt::to_byte(7.0), 255);
}
