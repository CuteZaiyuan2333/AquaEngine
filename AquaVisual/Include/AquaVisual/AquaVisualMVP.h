#pragma once

#include <string>

/**
 * @file AquaVisualMVP.h
 * @brief AquaVisual MVP - Minimum Viable Product Version
 * @version 0.1.0
 *
 * This header contains all core functionality for AquaVisual MVP
 */

// Core systems
#include "Core/Camera.h"
#include "Core/Renderer.h"
#include "Core/Window.h"

// 资源系统
#include "Resources/Mesh.h"
#include "Resources/Texture.h"

// 数学库
#include "Math.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

// 工具
#include "Primitives.h"

// 主要命名空间
namespace AquaVisual {

/**
 * @brief MVP版本信息
 */
struct MVPVersion {
  static constexpr int Major = 0;
  static constexpr int Minor = 1;
  static constexpr int Patch = 0;

  static std::string ToString() {
    return std::to_string(Major) + "." + std::to_string(Minor) + "." +
           std::to_string(Patch);
  }
};

/**
 * @brief 初始化AquaVisual MVP
 * @return 是否初始化成功
 */
bool InitializeMVP();

/**
 * @brief 清理AquaVisual MVP
 */
void ShutdownMVP();

/**
 * @brief 获取MVP版本字符串
 * @return 版本字符串
 */
std::string GetMVPVersion();

} // namespace AquaVisual

// Convenient macro definitions
#define AQUAVISUAL_MVP_VERSION "0.1.0"

// 常用类型别名
namespace AquaVisual {
using Vec2 = Vector2;
using Vec3 = Vector3;
using Vec4 = Vector4;
using Mat4 = Matrix4;
} // namespace AquaVisual