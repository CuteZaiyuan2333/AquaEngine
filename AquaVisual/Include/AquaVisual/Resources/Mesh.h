#pragma once

#include "../Math/Vector.h"
#include <memory>
#include <string>
#include <vector>

namespace AquaVisual {

/**
 * @brief Vertex structure
 */
struct Vertex {
  Vec3 position;
  Vec3 normal;
  Vec2 texCoord;

  Vertex() = default;
  Vertex(const Vec3 &pos, const Vec3 &norm = Vec3(0, 1, 0),
         const Vec2 &uv = Vec2(0, 0))
      : position(pos), normal(norm), texCoord(uv) {}
};

/**
 * @brief Mesh class - MVP version
 */
class Mesh {
public:
  /**
   * @brief Constructor
   * @param vertices Vertex data
   * @param indices Index data
   */
  Mesh(const std::vector<Vertex> &vertices,
       const std::vector<uint32_t> &indices);

  /**
   * @brief Destructor
   */
  virtual ~Mesh() = default;

  /**
   * @brief Get vertex data
   * @return Vertex data reference
   */
  const std::vector<Vertex> &GetVertices() const { return m_vertices; }

  /**
   * @brief Get index data
   * @return Index data reference
   */
  const std::vector<uint32_t> &GetIndices() const { return m_indices; }

  /**
   * @brief Get vertex count
   * @return Vertex count
   */
  size_t GetVertexCount() const { return m_vertices.size(); }

  /**
   * @brief Get index count
   * @return Index count
   */
  size_t GetIndexCount() const { return m_indices.size(); }

  /**
   * @brief Create cube mesh
   * @param size Cube size
   * @return Cube mesh
   */
  static std::unique_ptr<Mesh> CreateCube(float size = 1.0f);

  /**
   * @brief Create plane mesh
   * @param width Width
   * @param height Height
   * @return Plane mesh
   */
  static std::unique_ptr<Mesh> CreatePlane(float width = 1.0f,
                                           float height = 1.0f);

  /**
   * @brief Create triangle mesh
   * @param size Triangle size
   * @return Triangle mesh
   */
  static std::unique_ptr<Mesh> CreateTriangle(float size = 1.0f);

  /**
   * @brief Load mesh from file (simple OBJ format)
   * @param filepath File path
   * @return Mesh object
   */
  static std::unique_ptr<Mesh> LoadFromFile(const std::string &filepath);

protected:
  std::vector<Vertex> m_vertices;
  std::vector<uint32_t> m_indices;
};

} // namespace AquaVisual