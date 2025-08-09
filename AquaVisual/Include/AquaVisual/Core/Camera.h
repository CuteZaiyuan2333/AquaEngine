#pragma once

#include "../Math/Matrix.h"
#include "../Math/Vector.h"

namespace AquaVisual {

class Camera {
public:
    enum class ProjectionType {
        Perspective,
        Orthographic
    };

    Camera();
    Camera(const Vector3& position, const Vector3& target, const Vector3& up);

    void SetPerspective(float fov, float aspect, float nearPlane, float farPlane);
    void SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);

    void SetPosition(const Vector3& position);
    void SetTarget(const Vector3& target);
    void SetUp(const Vector3& up);
    void SetFOV(float fov);
    void SetAspectRatio(float aspectRatio);
    void SetNearPlane(float nearPlane);
    void SetFarPlane(float farPlane);
    void SetProjectionType(ProjectionType type);
    void SetOrthographicBounds(float left, float right, float bottom, float top);

    const Vector3& GetPosition() const;
    const Vector3& GetTarget() const;
    const Vector3& GetUp() const;
    float GetFOV() const;
    float GetAspectRatio() const;
    float GetNearPlane() const;
    float GetFarPlane() const;
    ProjectionType GetProjectionType() const;

    Vector3 GetForward() const;
    Vector3 GetRight() const;
    Vector3 GetUpVector() const;

    const Matrix4& GetViewMatrix() const;
    const Matrix4& GetProjectionMatrix() const;
    Matrix4 GetViewProjectionMatrix() const;

    void LookAt(const Vector3& position, const Vector3& target, const Vector3& up);
    void Move(const Vector3& offset);
    void MoveForward(float distance);
    void MoveRight(float distance);
    void MoveUp(float distance);
    void Rotate(float yaw, float pitch);
    void Orbit(const Vector3& center, float yaw, float pitch);
    void Zoom(float factor);

private:
    void UpdateViewMatrix();

    Vector3 m_position;
    Vector3 m_target;
    Vector3 m_up;

    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;

    ProjectionType m_projectionType;
    float m_left, m_right, m_bottom, m_top;

    mutable Matrix4 m_viewMatrix;
    mutable Matrix4 m_projectionMatrix;

    mutable bool m_viewMatrixDirty;
    mutable bool m_projectionMatrixDirty;
};

} // namespace AquaVisual