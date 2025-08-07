#pragma once

#include "Vector3.h"
#include <array>
#include <cmath>

namespace Aqua {
namespace Math {

class Matrix4 {
public:
    // 使用列主序存储 (OpenGL/Vulkan标准)
    std::array<float, 16> m;

    // 构造函数
    Matrix4() {
        Identity();
    }

    Matrix4(const std::array<float, 16>& values) : m(values) {}

    // 访问操作符
    float& operator()(int row, int col) {
        return m[col * 4 + row];
    }

    const float& operator()(int row, int col) const {
        return m[col * 4 + row];
    }

    // 矩阵运算
    Matrix4 operator*(const Matrix4& other) const {
        Matrix4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result(i, j) = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    result(i, j) += (*this)(i, k) * other(k, j);
                }
            }
        }
        return result;
    }

    // 向量变换
    Vector3 TransformPoint(const Vector3& point) const {
        float x = point.x * m[0] + point.y * m[4] + point.z * m[8] + m[12];
        float y = point.x * m[1] + point.y * m[5] + point.z * m[9] + m[13];
        float z = point.x * m[2] + point.y * m[6] + point.z * m[10] + m[14];
        float w = point.x * m[3] + point.y * m[7] + point.z * m[11] + m[15];
        
        if (w != 0.0f) {
            return Vector3(x / w, y / w, z / w);
        }
        return Vector3(x, y, z);
    }

    Vector3 TransformDirection(const Vector3& direction) const {
        float x = direction.x * m[0] + direction.y * m[4] + direction.z * m[8];
        float y = direction.x * m[1] + direction.y * m[5] + direction.z * m[9];
        float z = direction.x * m[2] + direction.y * m[6] + direction.z * m[10];
        return Vector3(x, y, z);
    }

    // 设置为单位矩阵
    void Identity() {
        m = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    // 平移矩阵
    static Matrix4 Translation(const Vector3& translation) {
        Matrix4 result;
        result.m[12] = translation.x;
        result.m[13] = translation.y;
        result.m[14] = translation.z;
        return result;
    }

    // 缩放矩阵
    static Matrix4 Scale(const Vector3& scale) {
        Matrix4 result;
        result.m[0] = scale.x;
        result.m[5] = scale.y;
        result.m[10] = scale.z;
        return result;
    }

    // 绕X轴旋转
    static Matrix4 RotationX(float angle) {
        Matrix4 result;
        float cos_a = std::cos(angle);
        float sin_a = std::sin(angle);
        
        result.m[5] = cos_a;
        result.m[6] = sin_a;
        result.m[9] = -sin_a;
        result.m[10] = cos_a;
        
        return result;
    }

    // 绕Y轴旋转
    static Matrix4 RotationY(float angle) {
        Matrix4 result;
        float cos_a = std::cos(angle);
        float sin_a = std::sin(angle);
        
        result.m[0] = cos_a;
        result.m[2] = -sin_a;
        result.m[8] = sin_a;
        result.m[10] = cos_a;
        
        return result;
    }

    // 绕Z轴旋转
    static Matrix4 RotationZ(float angle) {
        Matrix4 result;
        float cos_a = std::cos(angle);
        float sin_a = std::sin(angle);
        
        result.m[0] = cos_a;
        result.m[1] = sin_a;
        result.m[4] = -sin_a;
        result.m[5] = cos_a;
        
        return result;
    }

    // 透视投影矩阵
    static Matrix4 Perspective(float fov, float aspect, float near_plane, float far_plane) {
        Matrix4 result;
        result.m.fill(0.0f);
        
        float tan_half_fov = std::tan(fov * 0.5f);
        
        result.m[0] = 1.0f / (aspect * tan_half_fov);
        result.m[5] = 1.0f / tan_half_fov;
        result.m[10] = -(far_plane + near_plane) / (far_plane - near_plane);
        result.m[11] = -1.0f;
        result.m[14] = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
        
        return result;
    }

    // 视图矩阵 (LookAt)
    static Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
        Vector3 f = (center - eye).Normalized();
        Vector3 s = f.Cross(up).Normalized();
        Vector3 u = s.Cross(f);

        Matrix4 result;
        result.m[0] = s.x;
        result.m[4] = s.y;
        result.m[8] = s.z;
        result.m[1] = u.x;
        result.m[5] = u.y;
        result.m[9] = u.z;
        result.m[2] = -f.x;
        result.m[6] = -f.y;
        result.m[10] = -f.z;
        result.m[12] = -s.Dot(eye);
        result.m[13] = -u.Dot(eye);
        result.m[14] = f.Dot(eye);
        result.m[15] = 1.0f;

        return result;
    }

    // 获取数据指针（用于传递给GPU）
    const float* Data() const {
        return m.data();
    }
};

} // namespace Math
} // namespace Aqua