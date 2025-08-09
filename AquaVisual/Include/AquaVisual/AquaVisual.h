#pragma once

/**
 * @file AquaVisual.h
 * @brief AquaVisual Rendering Engine - Main Header
 * @version 1.0.0
 *
 * AquaVisual is a high-performance, modular Vulkan rendering engine
 * designed for modern graphics rendering and visualization.
 *
 * This header provides the main API for using AquaVisual in your applications.
 */

#include <memory>
#include <string>
#include <vector>

namespace AquaVisual {

/**
 * @brief AquaVisual version information
 */
struct Version {
  static constexpr int Major = 0;
  static constexpr int Minor = 1;
  static constexpr int Patch = 0;

  static std::string ToString() {
    return std::to_string(Major) + "." + std::to_string(Minor) + "." +
           std::to_string(Patch);
  }
};

/**
 * @brief Initialize AquaVisual library
 * @return True if initialization succeeded
 */
bool Initialize();

/**
 * @brief Shutdown AquaVisual library
 */
void Shutdown();

/**
 * @brief Get AquaVisual version string
 * @return Version string
 */
std::string GetVersion();

} // namespace AquaVisual

/**
 * @brief Convenience macros for common operations
 */
#define AQUAVISUAL_VERSION AquaVisual::Version::ToString()