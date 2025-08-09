#pragma once

#include "Math/Vector.h"
#include "Math/Matrix.h"

namespace AquaVisual {

// Forward declaration - Matrix4 is defined in Math/Matrix.h
class Matrix4;

/**
 * @brief Transform component
 */
struct Transform {
  Vector3 position = Vector3::Zero();
  Vector3 rotation = Vector3::Zero(); // Euler angles in radians
  Vector3 scale = Vector3::One();

  Transform() = default;
  Transform(const Vector3 &pos) : position(pos) {}
  Transform(const Vector3 &pos, const Vector3 &rot)
      : position(pos), rotation(rot) {}
  Transform(const Vector3 &pos, const Vector3 &rot, const Vector3 &scl)
      : position(pos), rotation(rot), scale(scl) {}

  // GetMatrix() implementation moved to source file to avoid circular dependency
  Matrix4 GetMatrix() const;
};

/**
 * @brief Math utility functions
 */
namespace Math {
constexpr float PI = 3.14159265359f;
constexpr float TWO_PI = 2.0f * PI;
constexpr float HALF_PI = PI * 0.5f;

inline float ToRadians(float degrees) { return degrees * PI / 180.0f; }
inline float ToDegrees(float radians) { return radians * 180.0f / PI; }

inline float Lerp(float a, float b, float t) { return a + t * (b - a); }
inline Vector3 Lerp(const Vector3 &a, const Vector3 &b, float t) {
  return a + (b - a) * t;
}

inline float Clamp(float value, float min, float max) {
  return value < min ? min : (value > max ? max : value);
}

inline Vector3 Clamp(const Vector3 &value, const Vector3 &min,
                     const Vector3 &max) {
  return {Clamp(value.x, min.x, max.x), Clamp(value.y, min.y, max.y),
          Clamp(value.z, min.z, max.z)};
}
} // namespace Math

} // namespace AquaVisual