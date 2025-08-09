#include "AquaVisual/Math/Matrix.h"
#include "AquaVisual/Math/Vector.h"
#include <cmath>

namespace AquaVisual {

Matrix4::Matrix4() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

Matrix4::Matrix4(float diagonal) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = (i == j) ? diagonal : 0.0f;
        }
    }
}

Matrix4::Matrix4(const float matrix[16]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = matrix[i * 4 + j];
        }
    }
}

Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 result(0.0f);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }
    return result;
}

Matrix4& Matrix4::operator*=(const Matrix4& other) {
    Matrix4 temp = *this;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                m[i][j] += temp.m[i][k] * other.m[k][j];
            }
        }
    }
    return *this;
}

float* Matrix4::Data() {
    return &m[0][0];
}

const float* Matrix4::Data() const {
    return &m[0][0];
}

Matrix4 Matrix4::Identity() {
    return Matrix4(1.0f);
}

Matrix4 Matrix4::Transpose() const {
    Matrix4 result(0.0f);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = m[j][i];
        }
    }
    return result;
}

Matrix4 Matrix4::Inverse() const {
    return Matrix4(1.0f);
}

Matrix4 Matrix4::Perspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
    Matrix4 result(0.0f);
    
    float tanHalfFov = tan(fov * 0.5f);
    
    result.m[0][0] = 1.0f / (aspectRatio * tanHalfFov);
    result.m[1][1] = 1.0f / tanHalfFov;
    result.m[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
    result.m[2][3] = -1.0f;
    result.m[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
    
    return result;
}

Matrix4 Matrix4::Orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
    Matrix4 result(0.0f);
    
    result.m[0][0] = 2.0f / (right - left);
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[2][2] = -2.0f / (farPlane - nearPlane);
    result.m[3][0] = -(right + left) / (right - left);
    result.m[3][1] = -(top + bottom) / (top - bottom);
    result.m[3][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
    result.m[3][3] = 1.0f;
    
    return result;
}

Matrix4 Matrix4::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
    Vector3 f = (center - eye).Normalize();
    Vector3 s = f.Cross(up).Normalize();
    Vector3 u = s.Cross(f);
    
    Matrix4 result(1.0f);
    result.m[0][0] = s.x;
    result.m[1][0] = s.y;
    result.m[2][0] = s.z;
    result.m[0][1] = u.x;
    result.m[1][1] = u.y;
    result.m[2][1] = u.z;
    result.m[0][2] = -f.x;
    result.m[1][2] = -f.y;
    result.m[2][2] = -f.z;
    result.m[3][0] = -s.Dot(eye);
    result.m[3][1] = -u.Dot(eye);
    result.m[3][2] = f.Dot(eye);
    
    return result;
}

}