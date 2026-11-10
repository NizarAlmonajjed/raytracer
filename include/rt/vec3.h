#pragma once

#include <cmath>
#include <ostream>

namespace rt {

// A 3-component vector of doubles. Used for points, directions and colours.
//
// The data members are public: Vec3 has no invariant to protect (any three
// doubles are a valid Vec3), so getters/setters would add noise, not safety.
class Vec3 {
public:
    double x{0.0};
    double y{0.0};
    double z{0.0};

    Vec3() = default;
    Vec3(double x_, double y_, double z_) : x{x_}, y{y_}, z{z_} {}

    // Unary minus: -v
    Vec3 operator-() const { return {-x, -y, -z}; }

    // Compound assignment operators modify *this and return a reference to it,
    // so they can be chained like built-in types: a += b += c;
    Vec3& operator+=(const Vec3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vec3& operator*=(double t) {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }

    Vec3& operator/=(double t) { return *this *= 1.0 / t; }

    double length_squared() const { return x * x + y * y + z * z; }
    double length() const { return std::sqrt(length_squared()); }
};

// Aliases make intent clear at the call site; they are the same type.
using Point3 = Vec3;
using Color = Vec3;

// ---- Free-function operators -------------------------------------------------
// Binary operators are written as non-member functions so that both operands
// are treated symmetrically (e.g. both 2.0 * v and v * 2.0 work).
// `inline` is required for functions defined in a header that is included in
// several .cpp files; it tells the linker the duplicates are the same function.

inline Vec3 operator+(Vec3 a, const Vec3& b) { return a += b; }
inline Vec3 operator-(Vec3 a, const Vec3& b) { return a -= b; }
inline Vec3 operator*(Vec3 v, double t) { return v *= t; }
inline Vec3 operator*(double t, Vec3 v) { return v *= t; }
inline Vec3 operator/(Vec3 v, double t) { return v /= t; }

// Component-wise multiply. Useful for colours: tinting light by a surface colour.
inline Vec3 operator*(const Vec3& a, const Vec3& b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}

inline double dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Returns a vector perpendicular to both a and b (right-hand rule).
//
//   cross(a, b) = ( a.y*b.z - a.z*b.y,
//                   a.z*b.x - a.x*b.z,
//                   a.x*b.y - a.y*b.x )
//
// Example: cross({1,0,0}, {0,1,0}) == {0,0,1}
inline Vec3 cross(const Vec3& a, const Vec3& b) {
    // TODO(you): implement using the formula above.
    // Tests: tests/vec3_test.cpp, the "Cross" tests.
    (void)a;
    (void)b;
    return {};
}

// Returns a vector with the same direction as v and length 1.
// Precondition: v is not the zero vector.
inline Vec3 unit_vector(const Vec3& v) { return v / v.length(); }

// Lets you write: std::cout << v;  -> "(1, 2, 3)"
inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
    return out << '(' << v.x << ", " << v.y << ", " << v.z << ')';
}

}  // namespace rt
