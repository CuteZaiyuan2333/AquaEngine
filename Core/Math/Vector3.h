#pragma once

#include <cmath>
#include <iostream>

namespace Aqua {
namespace Math {

class Vector3 {
public:
    float x, y, z;

    // Constructors
    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vector3(float value) : x(value), y(value), z(value) {}

    // Copy constructor
    Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) {}

    // Assignment operator
    Vector3& operator=(const Vector3& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }

    // Vector operations
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    Vector3 operator/(float scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vector3& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vector3& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vector3& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    // Dot product
    float Dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product
    Vector3 Cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Length
    float Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Length squared (avoids square root calculation)
    float LengthSquared() const {
        return x * x + y * y + z * z;
    }

    // Normalize
    Vector3 Normalized() const {
        float len = Length();
        if (len > 0.0f) {
            return *this / len;
        }
        return Vector3(0.0f);
    }

    // Normalize in place
    void Normalize() {
        float len = Length();
        if (len > 0.0f) {
            *this /= len;
        }
    }

    // Distance
    float Distance(const Vector3& other) const {
        return (*this - other).Length();
    }

    // Linear interpolation
    static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
        return a + (b - a) * t;
    }

    // Common vectors
    static const Vector3 Zero;
    static const Vector3 One;
    static const Vector3 Up;
    static const Vector3 Right;
    static const Vector3 Forward;

    // Output stream operator
    friend std::ostream& operator<<(std::ostream& os, const Vector3& v) {
        os << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }
};

// Scalar multiplication with scalar on the left
inline Vector3 operator*(float scalar, const Vector3& vector) {
    return vector * scalar;
}

} // namespace Math
} // namespace Aqua