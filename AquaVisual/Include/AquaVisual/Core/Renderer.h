#pragma once

#include <memory>
#include <string>
#include <vector>

namespace AquaVisual {

class Window;
class Camera;
class Mesh;
class Texture;

struct RendererConfig {
  uint32_t width = 800;
  uint32_t height = 600;
  std::string title = "AquaVisual MVP";
  bool enableValidation = true;
  bool enableVSync = true;
  uint32_t maxFramesInFlight = 2;
};

class Renderer {
public:
  static std::unique_ptr<Renderer> Create(const RendererConfig &config);

  virtual ~Renderer() = default;

  virtual bool Initialize() = 0;
  virtual void Shutdown() = 0;
  virtual bool BeginFrame() = 0;
  virtual void EndFrame() = 0;
  virtual void SetCamera(const Camera &camera) = 0;
  virtual void RenderMesh(const Mesh &mesh, const Texture *texture = nullptr) = 0;
  virtual void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) = 0;
  virtual bool ShouldClose() const = 0;
  virtual void PollEvents() = 0;
  virtual void GetWindowSize(uint32_t &width, uint32_t &height) const = 0;

protected:
  Renderer() = default;
};

} // namespace AquaVisual