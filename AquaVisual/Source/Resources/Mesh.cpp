#include "AquaVisual/Resources/Mesh.h"
#include <cmath>

namespace AquaVisual {

Mesh::Mesh(const std::vector<Vertex> &vertices,
           const std::vector<uint32_t> &indices)
    : m_vertices(vertices), m_indices(indices) {}

std::unique_ptr<Mesh> Mesh::CreateTriangle(float size) {
  std::vector<Vertex> vertices = {
      // 顶点位置                    法线              纹理坐标
      Vertex({0.0f, size, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.0f}),   // 顶部
      Vertex({-size, -size, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}), // 左下
      Vertex({size, -size, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f})   // 右下
  };

  std::vector<uint32_t> indices = {0, 1, 2};

  return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> Mesh::CreatePlane(float width, float height) {
  float halfWidth = width * 0.5f;
  float halfHeight = height * 0.5f;

  std::vector<Vertex> vertices = {
      // 顶点位置                           法线              纹理坐标
      Vertex({-halfWidth, -halfHeight, 0.0f}, {0.0f, 0.0f, 1.0f},
             {0.0f, 0.0f}), // 左下
      Vertex({halfWidth, -halfHeight, 0.0f}, {0.0f, 0.0f, 1.0f},
             {1.0f, 0.0f}), // 右下
      Vertex({halfWidth, halfHeight, 0.0f}, {0.0f, 0.0f, 1.0f},
             {1.0f, 1.0f}), // 右上
      Vertex({-halfWidth, halfHeight, 0.0f}, {0.0f, 0.0f, 1.0f},
             {0.0f, 1.0f}) // 左上
  };

  std::vector<uint32_t> indices = {
      0, 1, 2, // 第一个三角形
      2, 3, 0  // 第二个三角形
  };

  return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> Mesh::CreateCube(float size) {
  float halfSize = size * 0.5f;

  std::vector<Vertex> vertices = {
      // 前面 (Z+)
      Vertex({-halfSize, -halfSize, halfSize}, {0.0f, 0.0f, 1.0f},
             {0.0f, 0.0f}),
      Vertex({halfSize, -halfSize, halfSize}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}),
      Vertex({halfSize, halfSize, halfSize}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}),
      Vertex({-halfSize, halfSize, halfSize}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}),

      // 后面 (Z-)
      Vertex({halfSize, -halfSize, -halfSize}, {0.0f, 0.0f, -1.0f},
             {0.0f, 0.0f}),
      Vertex({-halfSize, -halfSize, -halfSize}, {0.0f, 0.0f, -1.0f},
             {1.0f, 0.0f}),
      Vertex({-halfSize, halfSize, -halfSize}, {0.0f, 0.0f, -1.0f},
             {1.0f, 1.0f}),
      Vertex({halfSize, halfSize, -halfSize}, {0.0f, 0.0f, -1.0f},
             {0.0f, 1.0f}),

      // 左面 (X-)
      Vertex({-halfSize, -halfSize, -halfSize}, {-1.0f, 0.0f, 0.0f},
             {0.0f, 0.0f}),
      Vertex({-halfSize, -halfSize, halfSize}, {-1.0f, 0.0f, 0.0f},
             {1.0f, 0.0f}),
      Vertex({-halfSize, halfSize, halfSize}, {-1.0f, 0.0f, 0.0f},
             {1.0f, 1.0f}),
      Vertex({-halfSize, halfSize, -halfSize}, {-1.0f, 0.0f, 0.0f},
             {0.0f, 1.0f}),

      // 右面 (X+)
      Vertex({halfSize, -halfSize, halfSize}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}),
      Vertex({halfSize, -halfSize, -halfSize}, {1.0f, 0.0f, 0.0f},
             {1.0f, 0.0f}),
      Vertex({halfSize, halfSize, -halfSize}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}),
      Vertex({halfSize, halfSize, halfSize}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}),

      // 底面 (Y-)
      Vertex({-halfSize, -halfSize, -halfSize}, {0.0f, -1.0f, 0.0f},
             {0.0f, 0.0f}),
      Vertex({halfSize, -halfSize, -halfSize}, {0.0f, -1.0f, 0.0f},
             {1.0f, 0.0f}),
      Vertex({halfSize, -halfSize, halfSize}, {0.0f, -1.0f, 0.0f},
             {1.0f, 1.0f}),
      Vertex({-halfSize, -halfSize, halfSize}, {0.0f, -1.0f, 0.0f},
             {0.0f, 1.0f}),

      // 顶面 (Y+)
      Vertex({-halfSize, halfSize, halfSize}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}),
      Vertex({halfSize, halfSize, halfSize}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}),
      Vertex({halfSize, halfSize, -halfSize}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}),
      Vertex({-halfSize, halfSize, -halfSize}, {0.0f, 1.0f, 0.0f},
             {0.0f, 1.0f})};

  std::vector<uint32_t> indices = {// 前面
                                   0, 1, 2, 2, 3, 0,
                                   // 后面
                                   4, 5, 6, 6, 7, 4,
                                   // 左面
                                   8, 9, 10, 10, 11, 8,
                                   // 右面
                                   12, 13, 14, 14, 15, 12,
                                   // 底面
                                   16, 17, 18, 18, 19, 16,
                                   // 顶面
                                   20, 21, 22, 22, 23, 20};

  return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> Mesh::LoadFromFile(const std::string &filepath) {
  // 简单的OBJ加载器实现（暂时返回一个三角形）
  // 在实际项目中，这里应该实现完整的OBJ文件解析
  return CreateTriangle(1.0f);
}

} // namespace AquaVisual