#include <stdexcept>
#include <thread>

#include <gtest/gtest.h>

#include "rt/options.h"

using rt::parse_options;

TEST(Options, DefaultsWithNoArguments) {
    const char* argv[] = {"raytracer"};
    const rt::Options o = parse_options(1, argv);
    EXPECT_EQ(o.width, 400);
    EXPECT_EQ(o.samples, 100);
    EXPECT_EQ(o.max_depth, 50);
    EXPECT_GE(o.threads, 1);
    EXPECT_EQ(o.output, "renders/materials.ppm");
    EXPECT_FALSE(o.show_help);
}

TEST(Options, ParsesEveryFlag) {
    const char* argv[] = {"raytracer", "--width", "800",  "--samples", "16",
                          "--depth",   "8",       "--threads", "3", "--output", "out.ppm"};
    const rt::Options o = parse_options(11, argv);
    EXPECT_EQ(o.width, 800);
    EXPECT_EQ(o.samples, 16);
    EXPECT_EQ(o.max_depth, 8);
    EXPECT_EQ(o.threads, 3);
    EXPECT_EQ(o.output, "out.ppm");
}

TEST(Options, Help) {
    const char* argv[] = {"raytracer", "--help"};
    EXPECT_TRUE(parse_options(2, argv).show_help);
}

TEST(Options, RejectsMissingValue) {
    const char* argv[] = {"raytracer", "--width"};
    EXPECT_THROW(parse_options(2, argv), std::invalid_argument);
}

TEST(Options, RejectsNonNumbers) {
    const char* a[] = {"raytracer", "--samples", "lots"};
    EXPECT_THROW(parse_options(3, a), std::invalid_argument);
    const char* b[] = {"raytracer", "--samples", "12abc"};
    EXPECT_THROW(parse_options(3, b), std::invalid_argument);
    const char* c[] = {"raytracer", "--samples", "99999999999999"};
    EXPECT_THROW(parse_options(3, c), std::invalid_argument);
}

TEST(Options, RejectsZeroAndNegative) {
    const char* a[] = {"raytracer", "--threads", "0"};
    EXPECT_THROW(parse_options(3, a), std::invalid_argument);
    const char* b[] = {"raytracer", "--width", "-5"};
    EXPECT_THROW(parse_options(3, b), std::invalid_argument);
}

TEST(Options, RejectsUnknownFlag) {
    const char* argv[] = {"raytracer", "--colour", "red"};
    EXPECT_THROW(parse_options(3, argv), std::invalid_argument);
}

TEST(Options, UsageMentionsEveryFlag) {
    const std::string u = rt::usage();
    for (const char* flag : {"--width", "--samples", "--depth", "--threads", "--output"}) {
        EXPECT_NE(u.find(flag), std::string::npos) << flag;
    }
}

// ---- Tests for your TODO: rt::default_thread_count --------------------------

TEST(DefaultThreads, MatchesHardwareWhenKnown) {
    const unsigned int hw = std::thread::hardware_concurrency();
    const int expected = hw == 0 ? 1 : static_cast<int>(hw);
    EXPECT_EQ(rt::default_thread_count(), expected);
}

TEST(DefaultThreads, IsAtLeastOne) {
    EXPECT_GE(rt::default_thread_count(), 1);
}
