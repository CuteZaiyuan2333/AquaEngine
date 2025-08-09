#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace AquaVisual {

// Forward declarations
class Texture;
class Shader;
class Mesh;
struct Transform;
struct Vector2;
struct Vector3;
class Matrix4;

/**
 * @brief Main rendering API interface
 *
 * This is the primary interface for all rendering operations in AquaVisual.
 * It provides a high-level, graphics API agnostic interface for rendering.
 */
class RenderAPI {
public:
  virtual ~RenderAPI() = default;

  // === Lifecycle ===

  /**
   * @brief Initialize the renderer
   * @return True if initialization succeeded
   */
  virtual bool Initialize() = 0;

  /**
   * @brief Shutdown the renderer
   */
  virtual void Shutdown() = 0;

  /**
   * @brief Check if the renderer should continue rendering
   * @return True if should continue rendering
   */
  virtual bool ShouldRender() const = 0;

  // === Frame Management ===

  /**
   * @brief Begin a new frame
   */
  virtual void BeginFrame() = 0;

  /**
   * @brief End the current frame and present
   */
  virtual void EndFrame() = 0;

  /**
   * @brief Set clear color
   * @param r Red component (0-1)
   * @param g Green component (0-1)
   * @param b Blue component (0-1)
   * @param a Alpha component (0-1)
   */
  virtual void SetClearColor(float r, float g, float b, float a = 1.0f) = 0;

  // === Resource Management ===

  /**
   * @brief Load texture from file
   * @param filepath Path to texture file
   * @return Shared pointer to texture
   */
  virtual std::shared_ptr<Texture> LoadTexture(const std::string &filepath) = 0;

  /**
   * @brief Create texture from raw data
   * @param data Raw texture data
   * @param width Texture width
   * @param height Texture height
   * @param channels Number of channels
   * @return Shared pointer to texture
   */
  virtual std::shared_ptr<Texture> CreateTexture(const uint8_t *data,
                                                 uint32_t width,
                                                 uint32_t height,
                                                 uint32_t channels) = 0;

  /**
   * @brief Load shader from file
   * @param vertexPath Path to vertex shader
   * @param fragmentPath Path to fragment shader
   * @return Shared pointer to shader
   */
  virtual std::shared_ptr<Shader>
  LoadShader(const std::string &vertexPath,
             const std::string &fragmentPath) = 0;

  /**
   * @brief Create mesh from vertex data
   * @param vertices Vertex data
   * @param indices Index data
   * @return Shared pointer to mesh
   */
  virtual std::shared_ptr<Mesh>
  CreateMesh(const std::vector<float> &vertices,
             const std::vector<uint32_t> &indices) = 0;

  // === Drawing Operations ===

  /**
   * @brief Draw a triangle
   * @param v1 First vertex position
   * @param v2 Second vertex position
   * @param v3 Third vertex position
   */
  virtual void DrawTriangle(const Vector2 &v1, const Vector2 &v2,
                            const Vector2 &v3) = 0;

  /**
   * @brief Draw a textured quad
   * @param texture Texture to use
   * @param position Quad position
   * @param size Quad size
   */
  virtual void DrawTexturedQuad(std::shared_ptr<Texture> texture,
                                const Vector2 &position,
                                const Vector2 &size) = 0;

  /**
   * @brief Draw a mesh with transform
   * @param mesh Mesh to draw
   * @param transform Transform matrix
   */
  virtual void DrawMesh(std::shared_ptr<Mesh> mesh,
                        const Transform &transform) = 0;

  /**
   * @brief Draw a mesh with custom shader
   * @param mesh Mesh to draw
   * @param shader Shader to use
   * @param transform Transform matrix
   */
  virtual void DrawMesh(std::shared_ptr<Mesh> mesh,
                        std::shared_ptr<Shader> shader,
                        const Transform &transform) = 0;

  // === Camera and View ===

  /**
   * @brief Set view matrix
   * @param view View matrix
   */
  virtual void SetViewMatrix(const Matrix4 &view) = 0;

  /**
   * @brief Set projection matrix
   * @param projection Projection matrix
   */
  virtual void SetProjectionMatrix(const Matrix4 &projection) = 0;

  /**
   * @brief Set viewport
   * @param x X offset
   * @param y Y offset
   * @param width Viewport width
   * @param height Viewport height
   */
  virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width,
                           uint32_t height) = 0;

  // === State Management ===

  /**
   * @brief Enable depth testing
   * @param enable True to enable depth testing
   */
  virtual void SetDepthTest(bool enable) = 0;

  /**
   * @brief Set wireframe mode
   * @param enable True to enable wireframe mode
   */
  virtual void SetWireframe(bool enable) = 0;

  /**
   * @brief Set culling mode
   * @param enable True to enable back-face culling
   */
  virtual void SetCulling(bool enable) = 0;

  // === Information ===

  /**
   * @brief Get renderer name
   * @return Renderer name string
   */
  virtual std::string GetRendererName() const = 0;

  /**
   * @brief Get GPU information
   * @return GPU information string
   */
  virtual std::string GetGPUInfo() const = 0;

  /**
   * @brief Get frame statistics
   * @return Frame time in milliseconds
   */
  virtual float GetFrameTime() const = 0;

  /**
   * @brief Get frames per second
   * @return Current FPS
   */
  virtual float GetFPS() const = 0;

  // === Event Callbacks ===

  /**
   * @brief Set window resize callback
   * @param callback Callback function
   */
  virtual void
  SetResizeCallback(std::function<void(uint32_t, uint32_t)> callback) = 0;

  /**
   * @brief Set key callback
   * @param callback Callback function
   */
  virtual void
  SetKeyCallback(std::function<void(int, int, int, int)> callback) = 0;

  /**
   * @brief Set mouse callback
   * @param callback Callback function
   */
  virtual void
  SetMouseCallback(std::function<void(double, double)> callback) = 0;
};

} // namespace AquaVisual