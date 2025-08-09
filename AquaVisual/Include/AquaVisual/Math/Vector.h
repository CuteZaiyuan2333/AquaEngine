#pragma once

#include <cmath>

namespace AquaVisual {

class Vector2 {
public:
    float x, y;

    Vector2();
    Vector2(float x, float y);
    Vector2(float value);

    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;
    Vector2& operator+=(const Vector2& other);
    Vector2& operator-=(const Vector2& other);
    Vector2& operator*=(float scalar);
    Vector2& operator/=(float scalar);

    float Dot(const Vector2& other) const;
    float Length() const;
    float LengthSquared() const;
    Vector2 Normalize() const;
    void NormalizeInPlace();

    Vector2 xy() const;
};

class Vector3 {
public:
    float x, y, z;

    Vector3();
    Vector3(float x, float y, float z);
    Vector3(float value);

    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;
    Vector3& operator+=(const Vector3& other);
    Vector3& operator-=(const Vector3& other);
    Vector3& operator*=(float scalar);
    Vector3& operator/=(float scalar);

    float Dot(const Vector3& other) const;
    Vector3 Cross(const Vector3& other) const;
    float Length() const;
    float LengthSquared() const;
    Vector3 Normalize() const;
    void NormalizeInPlace();

    Vector2 xy() const;
    Vector3 xyz() const;

    // Common vectors
    static Vector3 Zero() { return Vector3(0, 0, 0); }
    static Vector3 One() { return Vector3(1, 1, 1); }
    static Vector3 Up() { return Vector3(0, 1, 0); }
    static Vector3 Right() { return Vector3(1, 0, 0); }
    static Vector3 Forward() { return Vector3(0, 0, -1); }
};

class Vector4 {
public:
    float x, y, z, w;

    Vector4();
    Vector4(float x, float y, float z, float w);
    Vector4(const Vector3& xyz, float w);
    Vector4(float value);

    Vector4 operator+(const Vector4& other) const;
    Vector4 operator-(const Vector4& other) const;
    Vector4 operator*(float scalar) const;
    Vector4 operator/(float scalar) const;
    Vector4& operator+=(const Vector4& other);
    Vector4& operator-=(const Vector4& other);
    Vector4& operator*=(float scalar);
    Vector4& operator/=(float scalar);

    float Dot(const Vector4& other) const;
    float Length() const;
    float LengthSquared() const;
    Vector4 Normalize() const;
    void NormalizeInPlace();

    Vector2 xy() const;
    Vector3 xyz() const;
    Vector4 xyzw() const;
};

Vector2 operator*(float scalar, const Vector2& vec);
Vector3 operator*(float scalar, const Vector3& vec);
Vector4 operator*(float scalar, const Vector4& vec);

// Type aliases for convenience
using Vec2 = Vector2;
using Vec3 = Vector3;
using Vec4 = Vector4;

} // namespace AquaVisual