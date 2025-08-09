#pragma once

namespace AquaVisual {

class Vector3;
class Vector4;

class Matrix4 {
public:
    float m[4][4];

    Matrix4();
    Matrix4(float diagonal);
    Matrix4(const float matrix[16]);

    Matrix4 operator*(const Matrix4& other) const;
    Vector4 operator*(const Vector4& vec) const;
    Matrix4& operator*=(const Matrix4& other);

    float* Data();
    const float* Data() const;

    static Matrix4 Identity();
    static Matrix4 Translate(const Vector3& translation);
    static Matrix4 Scale(const Vector3& scale);
    static Matrix4 RotateX(float angle);
    static Matrix4 RotateY(float angle);
    static Matrix4 RotateZ(float angle);
    static Matrix4 Perspective(float fov, float aspect, float nearPlane, float farPlane);
    static Matrix4 Orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    static Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up);

    Matrix4 Transpose() const;
    Matrix4 Inverse() const;
};

}