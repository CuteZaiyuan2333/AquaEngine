#pragma once

#include <cmath>
#include <iostream>

namespace Aqua {
namespace Math {

class Vector2 {
public:
    float x, y;

    // Constructors
    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    Vector2(float value) : x(value), y(value) {}

    // Copy constructor
    Vector2(const Vector2& other) : x(other.x), y(other.y) {}

    // Assignment operator
    Vector2& operator=(const Vector2& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }

    // Vector operations
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 operator/(float scalar) const {
        return Vector2(x / scalar, y / scalar);
    }

    // Compound assignment operators
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // Comparison operators
    bool operator==(const Vector2& other) const {
        return (std::abs(x - other.x) < 1e-6f) && (std::abs(y - other.y) < 1e-6f);
    }

    bool operator!=(const Vector2& other) const {
        return !(*this == other);
    }

    // Dot product
    float Dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    // Length
    float Length() const {
        return std::sqrt(x * x + y * y);
    }

    // Length squared (avoids sqrt calculation)
    float LengthSquared() const {
        return x * x + y * y;
    }

    // Normalize
    Vector2 Normalized() const {
        float len = Length();
        if (len > 0.0f) {
            return *this / len;
        }
        return Vector2::Zero;
    }

    // Normalize in place
    void Normalize() {
        float len = Length();
        if (len > 0.0f) {
            *this /= len;
        }
    }

    // Distance
    float Distance(const Vector2& other) const {
        return (*this - other).Length();
    }

    // Linear interpolation
    static Vector2 Lerp(const Vector2& a, const Vector2& b, float t) {
        return a + (b - a) * t;
    }

    // Common vectors
    static const Vector2 Zero;
    static const Vector2 One;
    static const Vector2 Up;
    static const Vector2 Right;

    // Stream output operator
    friend std::ostream& operator<<(std::ostream& os, const Vector2& v) {
        os << "Vector2(" << v.x << ", " << v.y << ")";
        return os;
    }
};

// Scalar multiplication from left side
inline Vector2 operator*(float scalar, const Vector2& v) {
    return v * scalar;
}

} // namespace Math
} // namespace Aqua