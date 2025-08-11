#include "AquaVisual/Core/Camera.h"
#include <algorithm>
#include <cmath>

namespace AquaVisual {

Camera::Camera()
    : m_position(0.0f, 0.0f, 3.0f), m_target(0.0f, 0.0f, 0.0f),
      m_up(0.0f, 1.0f, 0.0f), m_fov(45.0f), m_aspectRatio(16.0f / 9.0f),
      m_nearPlane(0.1f), m_farPlane(100.0f),
      m_projectionType(ProjectionType::Perspective), m_left(-1.0f),
      m_right(1.0f), m_bottom(-1.0f), m_top(1.0f), m_viewMatrixDirty(true),
      m_projectionMatrixDirty(true) {}

Camera::Camera(const Vector3 &position, const Vector3 &target,
               const Vector3 &up)
    : m_position(position), m_target(target), m_up(up), m_fov(45.0f),
      m_aspectRatio(16.0f / 9.0f), m_nearPlane(0.1f), m_farPlane(100.0f),
      m_projectionType(ProjectionType::Perspective), m_left(-1.0f),
      m_right(1.0f), m_bottom(-1.0f), m_top(1.0f), m_viewMatrixDirty(true),
      m_projectionMatrixDirty(true) {}

void Camera::SetPosition(const Vector3 &position) {
  m_position = position;
  m_viewMatrixDirty = true;
}

void Camera::SetTarget(const Vector3 &target) {
  m_target = target;
  m_viewMatrixDirty = true;
}

void Camera::SetUp(const Vector3 &up) {
  m_up = up;
  m_viewMatrixDirty = true;
}

void Camera::SetFOV(float fov) {
  m_fov = std::max(1.0f, std::min(179.0f, fov));
  m_projectionMatrixDirty = true;
}

void Camera::SetAspectRatio(float aspectRatio) {
  m_aspectRatio = aspectRatio;
  m_projectionMatrixDirty = true;
}

void Camera::SetNearPlane(float nearPlane) {
  m_nearPlane = nearPlane;
  m_projectionMatrixDirty = true;
}

void Camera::SetFarPlane(float farPlane) {
  m_farPlane = farPlane;
  m_projectionMatrixDirty = true;
}

void Camera::SetProjectionType(ProjectionType type) {
  m_projectionType = type;
  m_projectionMatrixDirty = true;
}

void Camera::SetPerspective(float fov, float aspect, float nearPlane,
                            float farPlane) {
  m_fov = fov;
  m_aspectRatio = aspect;
  m_nearPlane = nearPlane;
  m_farPlane = farPlane;
  m_projectionType = ProjectionType::Perspective;
  m_projectionMatrixDirty = true;
}

void Camera::SetOrthographic(float left, float right, float bottom, float top,
                             float nearPlane, float farPlane) {
  m_left = left;
  m_right = right;
  m_bottom = bottom;
  m_top = top;
  m_nearPlane = nearPlane;
  m_farPlane = farPlane;
  m_projectionType = ProjectionType::Orthographic;
  m_projectionMatrixDirty = true;
}

void Camera::SetOrthographicBounds(float left, float right, float bottom,
                                   float top) {
  m_left = left;
  m_right = right;
  m_bottom = bottom;
  m_top = top;
  m_projectionMatrixDirty = true;
}

const Vector3 &Camera::GetPosition() const { return m_position; }

const Vector3 &Camera::GetTarget() const { return m_target; }

const Vector3 &Camera::GetUp() const { return m_up; }

float Camera::GetFOV() const { return m_fov; }

float Camera::GetAspectRatio() const { return m_aspectRatio; }

float Camera::GetNearPlane() const { return m_nearPlane; }

float Camera::GetFarPlane() const { return m_farPlane; }

Camera::ProjectionType Camera::GetProjectionType() const {
  return m_projectionType;
}

Vector3 Camera::GetForward() const {
  return (m_target - m_position).Normalize();
}

Vector3 Camera::GetRight() const {
  return GetForward().Cross(m_up).Normalize();
}

Vector3 Camera::GetUpVector() const {
  return GetRight().Cross(GetForward()).Normalize();
}

const Matrix4 &Camera::GetViewMatrix() const {
  if (m_viewMatrixDirty) {
    m_viewMatrix = Matrix4::LookAt(m_position, m_target, m_up);
    m_viewMatrixDirty = false;
  }
  return m_viewMatrix;
}

const Matrix4 &Camera::GetProjectionMatrix() const {
  if (m_projectionMatrixDirty) {
    if (m_projectionType == ProjectionType::Perspective) {
      float fovRadians = m_fov * 3.14159265359f / 180.0f;
      m_projectionMatrix = Matrix4::Perspective(fovRadians, m_aspectRatio,
                                                m_nearPlane, m_farPlane);
    } else {
      m_projectionMatrix = Matrix4::Orthographic(
          m_left, m_right, m_bottom, m_top, m_nearPlane, m_farPlane);
    }
    m_projectionMatrixDirty = false;
  }
  return m_projectionMatrix;
}

Matrix4 Camera::GetViewProjectionMatrix() const {
  return GetProjectionMatrix() * GetViewMatrix();
}

void Camera::LookAt(const Vector3 &position, const Vector3 &target,
                    const Vector3 &up) {
  m_position = position;
  m_target = target;
  m_up = up;
  m_viewMatrixDirty = true;
}

void Camera::Move(const Vector3 &offset) {
  m_position += offset;
  m_target += offset;
  m_viewMatrixDirty = true;
}

void Camera::MoveForward(float distance) {
  Vector3 forward = GetForward();
  m_position += forward * distance;
  m_target += forward * distance;
  m_viewMatrixDirty = true;
}

void Camera::MoveRight(float distance) {
  Vector3 right = GetRight();
  m_position += right * distance;
  m_target += right * distance;
  m_viewMatrixDirty = true;
}

void Camera::MoveUp(float distance) {
  Vector3 up = GetUpVector();
  m_position += up * distance;
  m_target += up * distance;
  m_viewMatrixDirty = true;
}

void Camera::Rotate(float yaw, float pitch) {
  Vector3 direction = m_target - m_position;
  float distance = direction.Length();

  float currentYaw = std::atan2(direction.z, direction.x);
  float currentPitch = std::asin(direction.y / distance);

  currentYaw += yaw;
  currentPitch += pitch;

  const float maxPitch = 1.5f;
  currentPitch = std::max(-maxPitch, std::min(maxPitch, currentPitch));

  direction.x = std::cos(currentPitch) * std::cos(currentYaw);
  direction.y = std::sin(currentPitch);
  direction.z = std::cos(currentPitch) * std::sin(currentYaw);

  m_target = m_position + direction * distance;
  m_viewMatrixDirty = true;
}

void Camera::Orbit(const Vector3 &center, float yaw, float pitch) {
  Vector3 positionOffset = m_position - center;
  float distance = positionOffset.Length();

  float currentYaw = std::atan2(positionOffset.z, positionOffset.x);
  float currentPitch = std::asin(positionOffset.y / distance);

  currentYaw += yaw;
  currentPitch += pitch;

  const float maxPitch = 1.5f;
  currentPitch = std::max(-maxPitch, std::min(maxPitch, currentPitch));

  positionOffset.x = std::cos(currentPitch) * std::cos(currentYaw);
  positionOffset.y = std::sin(currentPitch);
  positionOffset.z = std::cos(currentPitch) * std::sin(currentYaw);

  m_position = center + positionOffset * distance;
  m_target = center;
  m_viewMatrixDirty = true;
}

void Camera::Zoom(float factor) {
  if (m_projectionType == ProjectionType::Perspective) {
    SetFOV(m_fov * factor);
  } else {
    float width = m_right - m_left;
    float height = m_top - m_bottom;
    float centerX = (m_left + m_right) * 0.5f;
    float centerY = (m_bottom + m_top) * 0.5f;

    width *= factor;
    height *= factor;

    SetOrthographicBounds(centerX - width * 0.5f, centerX + width * 0.5f,
                          centerY - height * 0.5f, centerY + height * 0.5f);
  }
}

} // namespace AquaVisual