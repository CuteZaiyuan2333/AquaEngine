#pragma once

#include "Renderer.h"
#include <memory>

namespace AquaVisual {

class Window;
class Camera;
class Mesh;
class Texture;
class RenderPipeline;
class ShaderProgram;
class VertexBuffer;
class IndexBuffer;
class UniformBuffer;

/**
 * @brief Vulkan renderer implementation class
 */
class VulkanRendererImpl : public Renderer {
public:
  explicit VulkanRendererImpl(const RendererConfig &config);
  ~VulkanRendererImpl() override;

  // Renderer interface implementation
  bool Initialize() override;
  void Shutdown() override;
  bool BeginFrame() override;
  void EndFrame() override;
  void SetCamera(const Camera &camera) override;
  void RenderMesh(const Mesh &mesh, const Texture *texture = nullptr) override;
  void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f,
             float a = 1.0f) override;
  bool ShouldClose() const override;
  void PollEvents() override;
  void GetWindowSize(uint32_t &width, uint32_t &height) const override;

  // Get window object
  Window *GetWindow() const { return m_window.get(); }

private:
  RendererConfig m_config;
  std::unique_ptr<Window> m_window;
  const Camera *m_currentCamera;

  // Clear color
  float m_clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  
  // Rendering system state
  bool m_isInitialized = false;
  
  // Rendering resources
  std::shared_ptr<RenderPipeline> m_currentPipeline;
  std::shared_ptr<ShaderProgram> m_currentShaderProgram;
  std::shared_ptr<VertexBuffer> m_currentVertexBuffer;
  std::shared_ptr<IndexBuffer> m_currentIndexBuffer;
  std::shared_ptr<UniformBuffer> m_currentUniformBuffer;
  
  // Vulkan rendering system methods
  bool InitializeVulkan();
  void CleanupVulkan();
  void InitializeShaderManager();
  void CreateBasicPipeline();
  void CreateTestGeometry();
  
  // Rendering command methods
  void ClearScreen();
  void SubmitCommands();
  void BindPipeline(std::shared_ptr<RenderPipeline> pipeline);
  void BindVertexBuffer(std::shared_ptr<VertexBuffer> buffer);
  void BindIndexBuffer(std::shared_ptr<IndexBuffer> buffer);
  void DrawIndexed(uint32_t indexCount);
};

} // namespace AquaVisual