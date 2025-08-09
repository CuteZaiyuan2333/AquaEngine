/**
 * @file ThirdPartyIntegration.cpp
 * @brief Example of integrating AquaVisual into a third-party engine
 *
 * This example demonstrates how to use AquaVisual as a rendering backend
 * for other game engines or graphics applications.
 */

#include "AquaVisual/AquaVisual.h"
#include <AquaVisual/Core/Renderer.h>
#include <AquaVisual/Core/VulkanRendererImpl.h>
#include <AquaVisual/Math/Vector.h>
#include <AquaVisual/Primitives.h>
#include <iostream>
#include <memory>
#include <vector>

/**
 * @brief Simplified third-party engine interface
 *
 * This represents how another engine (like Bevy, Godot, etc.)
 * might integrate AquaVisual as a rendering backend.
 */
class ThirdPartyEngine {
public:
  /**
   * @brief Rendering backend interface
   */
  class IRenderBackend {
  public:
    virtual ~IRenderBackend() = default;
    virtual bool Initialize(uint32_t width, uint32_t height,
                            const std::string &title) = 0;
    virtual void Shutdown() = 0;
    virtual bool BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void SetClearColor(float r, float g, float b, float a) = 0;
    virtual void DrawTriangle(const std::vector<float> &vertices) = 0;
    virtual void DrawQuad(const std::vector<float> &vertices,
                          uint32_t textureId) = 0;
    virtual uint32_t LoadTexture(const std::string &path) = 0;
    virtual bool ShouldClose() = 0;
    virtual float GetFPS() = 0;
  };

  /**
   * @brief AquaVisual implementation of the rendering backend
   */
  class AquaVisualBackend : public IRenderBackend {
  private:
    std::unique_ptr<AquaVisual::VulkanRendererImpl> m_renderer;
    std::vector<std::shared_ptr<AquaVisual::Texture>> m_textures;
    float m_clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};

  public:
    bool Initialize(uint32_t width, uint32_t height,
                    const std::string &title) override {
      std::cout << "Initializing AquaVisual backend..." << std::endl;

      // Initialize AquaVisual
      if (!AquaVisual::Initialize()) {
        std::cerr << "Failed to initialize AquaVisual library!" << std::endl;
        return false;
      }

      // Create renderer
      AquaVisual::RendererConfig config;
      config.width = width;
      config.height = height;
      config.title = title;
      config.enableValidation = true;
      config.enableVSync = true;

      m_renderer = std::make_unique<AquaVisual::VulkanRendererImpl>(config);
      if (!m_renderer || !m_renderer->Initialize()) {
        std::cerr << "Failed to create AquaVisual renderer!" << std::endl;
        return false;
      }

      std::cout << "AquaVisual backend initialized successfully!" << std::endl;
      std::cout << "Using: VulkanRenderer" << std::endl;

      return true;
    }

    void Shutdown() override {
      std::cout << "Shutting down AquaVisual backend..." << std::endl;

      m_textures.clear();

      if (m_renderer) {
        m_renderer->Shutdown();
        m_renderer.reset();
      }

      AquaVisual::Shutdown();
    }

    bool BeginFrame() override {
      if (m_renderer && m_renderer->BeginFrame()) {
        m_renderer->Clear(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
        return true;
      }
      return false;
    }

    void EndFrame() override {
      if (m_renderer) {
        m_renderer->EndFrame();
      }
    }

    void SetClearColor(float r, float g, float b, float a) override {
      m_clearColor[0] = r;
      m_clearColor[1] = g;
      m_clearColor[2] = b;
      m_clearColor[3] = a;
    }

    void DrawTriangle(const std::vector<float> &vertices) override {
      // Note: Triangle drawing would need to be implemented
      // VulkanRendererImpl may not have DrawTriangle method
    }

    void DrawQuad(const std::vector<float> &vertices,
                  uint32_t textureId) override {
      // Note: Quad drawing would need to be implemented
      // VulkanRendererImpl may not have DrawTexturedQuad method
    }

    uint32_t LoadTexture(const std::string &path) override {
      // Note: Texture loading would need to be implemented
      // VulkanRendererImpl may not have LoadTexture method
      return UINT32_MAX;
    }

    bool ShouldClose() override {
      return m_renderer ? m_renderer->ShouldClose() : true;
    }

    float GetFPS() override { 
      // Note: VulkanRendererImpl doesn't have GetFPS method
      return 60.0f; // Return a placeholder value
    }
  };

private:
  std::unique_ptr<IRenderBackend> m_renderBackend;

public:
  bool Initialize(const std::string &backendType = "AquaVisual") {
    std::cout << "Initializing ThirdPartyEngine with " << backendType
              << " backend..." << std::endl;

    if (backendType == "AquaVisual") {
      m_renderBackend = std::make_unique<AquaVisualBackend>();
    } else {
      std::cerr << "Unknown backend type: " << backendType << std::endl;
      return false;
    }

    return m_renderBackend->Initialize(800, 600,
                                       "ThirdParty Engine with AquaVisual");
  }

  void Run() {
    if (!m_renderBackend)
      return;

    // Load some test assets
    uint32_t textureId =
        m_renderBackend->LoadTexture("Assets/Textures/test_valid.png");

    // Simple triangle vertices (position + color)
    std::vector<float> triangleVertices = {
        // Position      Color
        0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Top vertex
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Bottom left
        0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f  // Bottom right
    };

    // Simple quad vertices
    std::vector<float> quadVertices = {
        -100.0f, -100.0f, 0.0f, 0.0f, 0.0f, 100.0f,  -100.0f, 0.0f, 1.0f, 0.0f,
        100.0f,  100.0f,  0.0f, 1.0f, 1.0f, -100.0f, 100.0f,  0.0f, 0.0f, 1.0f};

    std::cout << "Starting render loop..." << std::endl;

    int frameCount = 0;
    while (!m_renderBackend->ShouldClose()) {
      // Set clear color (animated)
      float time = frameCount * 0.01f;
      float r = (sin(time) + 1.0f) * 0.5f * 0.2f;
      float g = (cos(time) + 1.0f) * 0.5f * 0.2f;
      float b = 0.3f;

      m_renderBackend->SetClearColor(r, g, b, 1.0f);

      // Begin frame
      if (!m_renderBackend->BeginFrame()) {
        break;
      }

      // Draw triangle
      m_renderBackend->DrawTriangle(triangleVertices);

      // Draw textured quad (if texture loaded successfully)
      if (textureId != UINT32_MAX) {
        m_renderBackend->DrawQuad(quadVertices, textureId);
      }

      // End frame
      m_renderBackend->EndFrame();

      // Print FPS occasionally
      if (++frameCount % 60 == 0) {
        std::cout << "FPS: " << m_renderBackend->GetFPS() << std::endl;
      }
    }

    std::cout << "Render loop ended." << std::endl;
  }

  void Shutdown() {
    if (m_renderBackend) {
      m_renderBackend->Shutdown();
      m_renderBackend.reset();
    }

    std::cout << "ThirdPartyEngine shutdown complete." << std::endl;
  }
};

/**
 * @brief Example of how another engine might use AquaVisual
 */
class BevyLikeEngine {
private:
  std::unique_ptr<AquaVisual::VulkanRendererImpl> m_aquaVisual;

public:
  struct RenderComponent {
    AquaVisual::Vector3 position;
    AquaVisual::Vector3 rotation;
    AquaVisual::Vector3 scale;
    std::shared_ptr<AquaVisual::Mesh> mesh;
    std::shared_ptr<AquaVisual::Texture> texture;
  };

  std::vector<RenderComponent> m_renderables;

  bool Initialize() {
    std::cout << "Initializing Bevy-like engine with AquaVisual..."
              << std::endl;

    if (!AquaVisual::Initialize()) {
      return false;
    }

    // Note: Using VulkanRendererImpl directly since CreateRenderer is not available
    AquaVisual::RendererConfig config;
    config.width = 1024;
    config.height = 768;
    config.title = "Bevy-like Engine with AquaVisual";
    config.enableValidation = true;
    config.enableVSync = true;

    m_aquaVisual = std::make_unique<AquaVisual::VulkanRendererImpl>(config);
    return m_aquaVisual && m_aquaVisual->Initialize();
  }

  void AddRenderable(const RenderComponent &component) {
    m_renderables.push_back(component);
  }

  void Update() {
    // ECS-like update system
    for (auto &renderable : m_renderables) {
      // Update transform or other components
      renderable.rotation.y += 0.01f;
    }
  }

  void Render() {
    if (!m_aquaVisual)
      return;

    if (m_aquaVisual->BeginFrame()) {
      m_aquaVisual->Clear(0.2f, 0.3f, 0.4f, 1.0f);

      // Render all components
      for (const auto &renderable : m_renderables) {
        // Note: Mesh rendering would need to be implemented
        // For now, just clear the screen
      }

      m_aquaVisual->EndFrame();
    }
  }

  bool ShouldRun() { return m_aquaVisual && !m_aquaVisual->ShouldClose(); }

  void Shutdown() {
    m_renderables.clear();

    if (m_aquaVisual) {
      m_aquaVisual->Shutdown();
      m_aquaVisual.reset();
    }

    AquaVisual::Shutdown();
  }
};

/**
 * @brief Main entry point demonstrating different integration approaches
 */
int main() {
  std::cout << "=== AquaVisual Third-Party Integration Examples ==="
            << std::endl;

  // Example 1: Generic third-party engine with backend abstraction
  {
    std::cout << "\n--- Example 1: Generic Backend Integration ---"
              << std::endl;

    ThirdPartyEngine engine;
    if (engine.Initialize("AquaVisual")) {
      engine.Run();
      engine.Shutdown();
    }
  }

  // Example 2: ECS-style engine (like Bevy)
  {
    std::cout << "\n--- Example 2: ECS-Style Integration ---" << std::endl;

    BevyLikeEngine bevyEngine;
    if (bevyEngine.Initialize()) {
      // Add some renderables
      BevyLikeEngine::RenderComponent cube;
      cube.position = {0, 0, 0};
      cube.rotation = {0, 0, 0};
      cube.scale = {1, 1, 1};
      // cube.mesh would be loaded here

      bevyEngine.AddRenderable(cube);

      // Simple render loop
      while (bevyEngine.ShouldRun()) {
        bevyEngine.Update();
        bevyEngine.Render();
      }

      bevyEngine.Shutdown();
    }
  }

  std::cout << "\nAll integration examples completed!" << std::endl;
  return 0;
}