#include "AquaVisual/Math/Vector.h"
#include <cmath>

namespace AquaVisual {

// Vector2 implementation
Vector2::Vector2() : x(0.0f), y(0.0f) {}

Vector2::Vector2(float x, float y) : x(x), y(y) {}

Vector2::Vector2(float value) : x(value), y(value) {}

Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const {
    return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(float scalar) const {
    return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator/(float scalar) const {
    return Vector2(x / scalar, y / scalar);
}

Vector2& Vector2::operator+=(const Vector2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2& Vector2::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2& Vector2::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
}

float Vector2::Dot(const Vector2& other) const {
    return x * other.x + y * other.y;
}

float Vector2::Length() const {
    return std::sqrt(x * x + y * y);
}

float Vector2::LengthSquared() const {
    return x * x + y * y;
}

Vector2 Vector2::Normalize() const {
    float len = Length();
    if (len > 0.0f) {
        return *this / len;
    }
    return Vector2();
}

void Vector2::NormalizeInPlace() {
    float len = Length();
    if (len > 0.0f) {
        *this /= len;
    }
}

Vector2 Vector2::xy() const {
    return *this;
}

// Vector3 implementation
Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

Vector3::Vector3(float value) : x(value), y(value), z(value) {}

Vector3 Vector3::operator+(const Vector3& other) const {
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator-(const Vector3& other) const {
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::operator*(float scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator/(float scalar) const {
    return Vector3(x / scalar, y / scalar, z / scalar);
}

Vector3& Vector3::operator+=(const Vector3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vector3& Vector3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3& Vector3::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

float Vector3::Dot(const Vector3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vector3 Vector3::Cross(const Vector3& other) const {
    return Vector3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

float Vector3::Length() const {
    return std::sqrt(x * x + y * y + z * z);
}

float Vector3::LengthSquared() const {
    return x * x + y * y + z * z;
}

Vector3 Vector3::Normalize() const {
    float len = Length();
    if (len > 0.0f) {
        return *this / len;
    }
    return Vector3();
}

void Vector3::NormalizeInPlace() {
    float len = Length();
    if (len > 0.0f) {
        *this /= len;
    }
}

Vector2 Vector3::xy() const {
    return Vector2(x, y);
}

Vector3 Vector3::xyz() const {
    return *this;
}

// Vector4 implementation
Vector4::Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

Vector4::Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

Vector4::Vector4(const Vector3& xyz, float w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}

Vector4::Vector4(float value) : x(value), y(value), z(value), w(value) {}

Vector4 Vector4::operator+(const Vector4& other) const {
    return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
}

Vector4 Vector4::operator-(const Vector4& other) const {
    return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
}

Vector4 Vector4::operator*(float scalar) const {
    return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
}

Vector4 Vector4::operator/(float scalar) const {
    return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
}

Vector4& Vector4::operator+=(const Vector4& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
}

Vector4& Vector4::operator-=(const Vector4& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
}

Vector4& Vector4::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
}

Vector4& Vector4::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;
    return *this;
}

float Vector4::Dot(const Vector4& other) const {
    return x * other.x + y * other.y + z * other.z + w * other.w;
}

float Vector4::Length() const {
    return std::sqrt(x * x + y * y + z * z + w * w);
}

float Vector4::LengthSquared() const {
    return x * x + y * y + z * z + w * w;
}

Vector4 Vector4::Normalize() const {
    float len = Length();
    if (len > 0.0f) {
        return *this / len;
    }
    return Vector4();
}

void Vector4::NormalizeInPlace() {
    float len = Length();
    if (len > 0.0f) {
        *this /= len;
    }
}

Vector3 Vector4::xyz() const {
    return Vector3(x, y, z);
}

Vector2 Vector4::xy() const {
    return Vector2(x, y);
}

Vector4 Vector4::xyzw() const {
    return *this;
}

// Global operator overloads
Vector2 operator*(float scalar, const Vector2& vec) {
    return vec * scalar;
}

Vector3 operator*(float scalar, const Vector3& vec) {
    return vec * scalar;
}

Vector4 operator*(float scalar, const Vector4& vec) {
    return vec * scalar;
}

} // namespace AquaVisual