#pragma once

#include "Resources/Mesh.h"
#include <memory>

namespace AquaVisual {

class Renderer;

/**
 * @brief Basic geometry creation utilities
 */
namespace Primitives {

/**
 * @brief Create triangle mesh
 * @param size Triangle size
 * @return Triangle mesh
 */
std::unique_ptr<Mesh> CreateTriangle(float size = 1.0f);

/**
 * @brief Create quad mesh
 * @param width Width
 * @param height Height
 * @return Quad mesh
 */
std::unique_ptr<Mesh> CreateQuad(float width = 1.0f, float height = 1.0f);

/**
 * @brief Create cube mesh
 * @param size Cube size
 * @return Cube mesh
 */
std::unique_ptr<Mesh> CreateCube(float size = 1.0f);

/**
 * @brief Create sphere mesh
 * @param radius Sphere radius
 * @param segments Segment count
 * @return Sphere mesh
 */
std::unique_ptr<Mesh> CreateSphere(float radius = 1.0f, int segments = 32);

/**
 * @brief Create plane mesh
 * @param width Width
 * @param height Height
 * @param widthSegments Width segments
 * @param heightSegments Height segments
 * @return Plane mesh
 */
std::unique_ptr<Mesh> CreatePlane(float width = 1.0f, float height = 1.0f,
                                  int widthSegments = 1,
                                  int heightSegments = 1);

} // namespace Primitives

} // namespace AquaVisual