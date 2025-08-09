#include "AquaVisual/Primitives.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace AquaVisual {
namespace Primitives {

std::unique_ptr<Mesh> CreateTriangle(float size) {
  return Mesh::CreateTriangle(size);
}

std::unique_ptr<Mesh> CreateQuad(float width, float height) {
  return Mesh::CreatePlane(width, height);
}

std::unique_ptr<Mesh> CreateCube(float size) { return Mesh::CreateCube(size); }

std::unique_ptr<Mesh> CreateSphere(float radius, int segments) {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  // 生成球体顶点
  for (int lat = 0; lat <= segments; ++lat) {
    float theta = lat * M_PI / segments;
    float sinTheta = sin(theta);
    float cosTheta = cos(theta);

    for (int lon = 0; lon <= segments; ++lon) {
      float phi = lon * 2 * M_PI / segments;
      float sinPhi = sin(phi);
      float cosPhi = cos(phi);

      Vector3 position(radius * sinTheta * cosPhi, radius * cosTheta,
                       radius * sinTheta * sinPhi);

      Vector3 normal = position.Normalize();
      Vector2 texCoord((float)lon / segments, (float)lat / segments);

      vertices.emplace_back(position, normal, texCoord);
    }
  }

  // 生成球体索引
  for (int lat = 0; lat < segments; ++lat) {
    for (int lon = 0; lon < segments; ++lon) {
      int current = lat * (segments + 1) + lon;
      int next = current + segments + 1;

      // 第一个三角形
      indices.push_back(current);
      indices.push_back(next);
      indices.push_back(current + 1);

      // 第二个三角形
      indices.push_back(current + 1);
      indices.push_back(next);
      indices.push_back(next + 1);
    }
  }

  return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> CreatePlane(float width, float height, int widthSegments,
                                  int heightSegments) {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  float halfWidth = width * 0.5f;
  float halfHeight = height * 0.5f;

  // 生成顶点
  for (int y = 0; y <= heightSegments; ++y) {
    for (int x = 0; x <= widthSegments; ++x) {
      float u = (float)x / widthSegments;
      float v = (float)y / heightSegments;

      Vector3 position((u - 0.5f) * width, (v - 0.5f) * height, 0.0f);

      Vector3 normal(0.0f, 0.0f, 1.0f);
      Vector2 texCoord(u, v);

      vertices.emplace_back(position, normal, texCoord);
    }
  }

  // 生成索引
  for (int y = 0; y < heightSegments; ++y) {
    for (int x = 0; x < widthSegments; ++x) {
      int topLeft = y * (widthSegments + 1) + x;
      int topRight = topLeft + 1;
      int bottomLeft = (y + 1) * (widthSegments + 1) + x;
      int bottomRight = bottomLeft + 1;

      // 第一个三角形
      indices.push_back(topLeft);
      indices.push_back(bottomLeft);
      indices.push_back(topRight);

      // 第二个三角形
      indices.push_back(topRight);
      indices.push_back(bottomLeft);
      indices.push_back(bottomRight);
    }
  }

  return std::make_unique<Mesh>(vertices, indices);
}

} // namespace Primitives
} // namespace AquaVisual