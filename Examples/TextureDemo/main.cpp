#include "../../Core/Math/Matrix4.h"
#include "../../Core/Math/Vector3.h"
#include "../../Core/Platform/Application.h"
#include "../../Core/Renderer/Renderer.h"
#include "../../Core/Renderer/Texture.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

class TextureDemoApp : public Aqua::Platform::Application {
private:
  std::unique_ptr<Aqua::Renderer::Renderer> m_renderer;
  std::vector<std::unique_ptr<AquaEngine::Texture>> m_textures;
  std::vector<std::string> m_textureNames;
  int m_currentTextureIndex;
  float m_rotationAngle;
  bool m_shouldExit;

public:
  TextureDemoApp()
      : m_currentTextureIndex(0), m_rotationAngle(0.0f), m_shouldExit(false) {}

  bool Initialize() override {
    // Call base class initialization method
    if (!Aqua::Platform::Application::Initialize()) {
      std::cerr << "Base class initialization failed" << std::endl;
      return false;
    }

    std::cout << "=== AquaEngine Texture Demo Application ===" << std::endl;
    std::cout << "Initializing texture demo..." << std::endl;

    // Create renderer
    m_renderer = std::make_unique<Aqua::Renderer::Renderer>();
    if (!m_renderer->Initialize(GetVulkanContext())) {
      std::cerr << "Renderer initialization failed" << std::endl;
      return false;
    }

    // Load test textures
    LoadTestTextures();

    // Set up keyboard input handling
    GetWindow()->SetKeyCallback(
        [this](int key, int action) { OnKeyInput(key, action); });

    std::cout << "Texture demo initialized successfully!" << std::endl;
    std::cout << "Window should be visible now..." << std::endl;
    std::cout << "Press ESC to exit, SPACE to cycle through textures"
              << std::endl;

    return true;
  }

  void OnKeyInput(int key, int action) {
    if (action == GLFW_PRESS) {
      switch (key) {
      case GLFW_KEY_ESCAPE:
        std::cout << "ESC pressed - exiting..." << std::endl;
        m_shouldExit = true;
        break;
      case GLFW_KEY_SPACE:
        if (!m_textures.empty()) {
          m_currentTextureIndex =
              (m_currentTextureIndex + 1) % m_textures.size();
          std::cout << "SPACE pressed - switched to: "
                    << m_textureNames[m_currentTextureIndex] << std::endl;
        }
        break;
      }
    }
  }

  void LoadTestTextures() {
    // Set VulkanContext for texture operations
    AquaEngine::Texture::SetVulkanContext(GetVulkanContext().get());

    std::vector<std::string> textureFiles = {
        "Assets/Textures/test_red_4x4.png",
        "Assets/Textures/test_green_8x8.png"};

    std::vector<std::string> textureDescriptions = {"Red 4x4 Texture",
                                                    "Green 8x8 Texture"};

    std::cout << "Loading textures..." << std::endl;

    for (size_t i = 0; i < textureFiles.size(); ++i) {
      auto texture = std::make_unique<AquaEngine::Texture>();
      if (texture->LoadFromFile(textureFiles[i])) {
        m_textures.push_back(std::move(texture));
        m_textureNames.push_back(textureDescriptions[i]);
        std::cout << "Loaded: " << textureDescriptions[i] << std::endl;
      } else {
        std::cout << "Failed to load: " << textureFiles[i] << std::endl;
      }
    }

    if (m_textures.empty()) {
      std::cout << "Warning: No textures loaded successfully!" << std::endl;
      // Add a fallback message
      m_textureNames.push_back("No Texture Loaded");
    } else {
      // Set the first texture as current immediately after loading
      auto &firstTexture = m_textures[0];
      m_renderer->SetCurrentTexture(firstTexture->GetImageView(),
                                    firstTexture->GetSampler());
      std::cout << "Set initial texture: " << m_textureNames[0] << std::endl;
    }

    std::cout << "Total textures loaded: " << m_textures.size() << std::endl;
  }

  void Update() override {
    // Check if we should exit
    if (m_shouldExit) {
      GetWindow()->SetShouldClose(true);
      return;
    }

    // Rotate the texture display
    m_rotationAngle += 0.01f;
    if (m_rotationAngle > 6.28f) { // 2 * PI
      m_rotationAngle = 0.0f;
    }
  }

  void Render() override {
    if (m_renderer) {
      if (m_renderer->BeginFrame()) {
        // Render background triangle with current texture
        RenderTexturedTriangle();
        m_renderer->EndFrame();
      } else {
        // If BeginFrame fails, add a small delay to prevent tight loops
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
      }
    }
  }

  void RenderTexturedTriangle() {
    // Bind current texture if available
    if (!m_textures.empty() && m_currentTextureIndex < m_textures.size()) {
      auto &texture = m_textures[m_currentTextureIndex];
      m_renderer->SetCurrentTexture(texture->GetImageView(),
                                    texture->GetSampler());
    }

    // Render the textured triangle
    m_renderer->RenderTriangle();

    // Display current texture info in console
    static int lastDisplayedIndex = -1;
    if (lastDisplayedIndex != m_currentTextureIndex) {
      lastDisplayedIndex = m_currentTextureIndex;
      if (!m_textures.empty() && m_currentTextureIndex < m_textures.size()) {
        auto &texture = m_textures[m_currentTextureIndex];
        std::cout << "Displaying: " << m_textureNames[m_currentTextureIndex]
                  << " (" << texture->GetWidth() << "x" << texture->GetHeight()
                  << ")" << std::endl;
      }
    }
  }

  void Cleanup() override {
    std::cout << "Texture demo cleanup" << std::endl;

    // Cleanup textures
    m_textures.clear();
    m_textureNames.clear();

    // Cleanup renderer
    if (m_renderer) {
      m_renderer->Cleanup();
      m_renderer.reset();
    }

    // Call base class cleanup method
    Aqua::Platform::Application::Cleanup();

    std::cout << "Texture demo cleanup complete" << std::endl;
  }
};

int main() {
  std::cout << "Starting AquaEngine Texture Demo..." << std::endl;
  std::cout << "This demo will show a window with texture loading capabilities."
            << std::endl;
  std::cout << "========================================" << std::endl;

  TextureDemoApp app;
  app.Run();

  std::cout << "Texture demo finished." << std::endl;
  return 0;
}