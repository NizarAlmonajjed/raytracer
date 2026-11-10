#include <sstream>

#include <gtest/gtest.h>

#include "rt/vec3.h"
#include "test_helpers.h"

using rt::Vec3;
using rt_test::VecNear;

TEST(Vec3, DefaultIsZero) {
    EXPECT_TRUE(VecNear(Vec3{}, Vec3{0, 0, 0}));
}

TEST(Vec3, Addition) {
    EXPECT_TRUE(VecNear(Vec3{1, 2, 3} + Vec3{4, 5, 6}, Vec3{5, 7, 9}));
}

TEST(Vec3, Subtraction) {
    EXPECT_TRUE(VecNear(Vec3{4, 5, 6} - Vec3{1, 2, 3}, Vec3{3, 3, 3}));
}

TEST(Vec3, Negation) {
    EXPECT_TRUE(VecNear(-Vec3{1, -2, 3}, Vec3{-1, 2, -3}));
}

TEST(Vec3, ScalarMultiplyBothSides) {
    EXPECT_TRUE(VecNear(Vec3{1, 2, 3} * 2.0, Vec3{2, 4, 6}));
    EXPECT_TRUE(VecNear(2.0 * Vec3{1, 2, 3}, Vec3{2, 4, 6}));
}

TEST(Vec3, ScalarDivide) {
    EXPECT_TRUE(VecNear(Vec3{2, 4, 6} / 2.0, Vec3{1, 2, 3}));
}

TEST(Vec3, ComponentWiseMultiply) {
    EXPECT_TRUE(VecNear(Vec3{1, 2, 3} * Vec3{4, 5, 6}, Vec3{4, 10, 18}));
}

TEST(Vec3, CompoundAssignmentModifiesInPlace) {
    Vec3 v{1, 1, 1};
    v += Vec3{1, 2, 3};
    EXPECT_TRUE(VecNear(v, Vec3{2, 3, 4}));
    v *= 2.0;
    EXPECT_TRUE(VecNear(v, Vec3{4, 6, 8}));
    v /= 4.0;
    EXPECT_TRUE(VecNear(v, Vec3{1, 1.5, 2}));
    v -= Vec3{1, 1, 1};
    EXPECT_TRUE(VecNear(v, Vec3{0, 0.5, 1}));
}

TEST(Vec3, Length) {
    EXPECT_DOUBLE_EQ(Vec3(3, 4, 0).length(), 5.0);
    EXPECT_DOUBLE_EQ(Vec3(1, 2, 2).length_squared(), 9.0);
}

TEST(Vec3, DotProduct) {
    EXPECT_DOUBLE_EQ(rt::dot(Vec3{1, 2, 3}, Vec3{4, 5, 6}), 32.0);
}

TEST(Vec3, DotOfPerpendicularVectorsIsZero) {
    EXPECT_DOUBLE_EQ(rt::dot(Vec3{1, 0, 0}, Vec3{0, 1, 0}), 0.0);
}

TEST(Vec3, UnitVectorHasLengthOneAndSameDirection) {
    const Vec3 u = rt::unit_vector(Vec3{3, 0, 4});
    EXPECT_DOUBLE_EQ(u.length(), 1.0);
    EXPECT_TRUE(VecNear(u, Vec3{0.6, 0, 0.8}));
}

TEST(Vec3, StreamOutput) {
    std::ostringstream out;
    out << Vec3{1, 2.5, -3};
    EXPECT_EQ(out.str(), "(1, 2.5, -3)");
}

// ---- Tests for your TODO: rt::cross -----------------------------------------

TEST(Cross, XCrossYIsZ) {
    EXPECT_TRUE(VecNear(rt::cross(Vec3{1, 0, 0}, Vec3{0, 1, 0}), Vec3{0, 0, 1}));
}

TEST(Cross, OrderMattersYCrossXIsMinusZ) {
    EXPECT_TRUE(VecNear(rt::cross(Vec3{0, 1, 0}, Vec3{1, 0, 0}), Vec3{0, 0, -1}));
}

TEST(Cross, GeneralCase) {
    EXPECT_TRUE(VecNear(rt::cross(Vec3{1, 2, 3}, Vec3{4, 5, 6}), Vec3{-3, 6, -3}));
}

TEST(Cross, ResultIsPerpendicularToBothInputs) {
    const Vec3 a{2, -1, 5};
    const Vec3 b{0.5, 3, -2};
    const Vec3 c = rt::cross(a, b);
    EXPECT_NEAR(rt::dot(c, a), 0.0, 1e-9);
    EXPECT_NEAR(rt::dot(c, b), 0.0, 1e-9);
    EXPECT_GT(c.length(), 0.0);  // and it isn't just the zero vector
}
