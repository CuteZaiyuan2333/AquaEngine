/**
 * @file AquaEngineIntegration.cpp
 * @brief Example of integrating AquaVisual into AquaEngine
 *
 * This example demonstrates how to use AquaVisual as an independent
 * rendering engine within the AquaEngine game engine framework.
 */

#include "AquaVisual/AquaVisual.h"
#include <iostream>
#include <memory>

/**
 * @brief Simple game engine class that uses AquaVisual for rendering
 */
class AquaEngine {
private:
  std::unique_ptr<AquaVisual::RenderAPI> m_renderer;
  std::shared_ptr<AquaVisual::Texture> m_texture;
  std::shared_ptr<AquaVisual::Mesh> m_cubeMesh;

  AquaVisual::Transform m_cubeTransform;
  float m_rotationSpeed = 1.0f;
  float m_time = 0.0f;

public:
  bool Initialize() {
    std::cout << "Initializing AquaEngine with AquaVisual v"
              << AquaVisual::GetVersion() << std::endl;

    // Check if Vulkan is available
    if (!AquaVisual::IsVulkanAvailable()) {
      std::cerr << "Vulkan is not available!" << std::endl;
      return false;
    }

    // Initialize AquaVisual library
    if (!AquaVisual::Initialize()) {
      std::cerr << "Failed to initialize AquaVisual!" << std::endl;
      return false;
    }

    // Create renderer with custom configuration
    AquaVisual::RendererConfig config;
    config.width = 1024;
    config.height = 768;
    config.title = "AquaEngine with AquaVisual Renderer";
    config.enableValidation = true;
    config.enableVSync = true;

    m_renderer = AquaVisual::CreateRenderer(config);
    if (!m_renderer) {
      std::cerr << "Failed to create AquaVisual renderer!" << std::endl;
      return false;
    }

    if (!m_renderer->Initialize()) {
      std::cerr << "Failed to initialize renderer!" << std::endl;
      return false;
    }

    std::cout << "Renderer: " << m_renderer->GetRendererName() << std::endl;
    std::cout << "GPU: " << m_renderer->GetGPUInfo() << std::endl;

    // Load game assets
    if (!LoadAssets()) {
      std::cerr << "Failed to load assets!" << std::endl;
      return false;
    }

    // Setup camera
    SetupCamera();

    // Setup input callbacks
    SetupInput();

    return true;
  }

  void Run() {
    std::cout << "Starting game loop..." << std::endl;

    while (m_renderer->ShouldRender()) {
      Update();
      Render();
    }

    std::cout << "Game loop ended." << std::endl;
  }

  void Shutdown() {
    std::cout << "Shutting down AquaEngine..." << std::endl;

    // Clean up resources
    m_texture.reset();
    m_cubeMesh.reset();

    // Shutdown renderer
    if (m_renderer) {
      m_renderer->Shutdown();
      m_renderer.reset();
    }

    // Shutdown AquaVisual library
    AquaVisual::Shutdown();

    std::cout << "AquaEngine shutdown complete." << std::endl;
  }

private:
  bool LoadAssets() {
    // Load texture
    m_texture = m_renderer->LoadTexture("Assets/Textures/test_valid.png");
    if (!m_texture) {
      std::cerr << "Failed to load texture!" << std::endl;
      return false;
    }

    // Create cube mesh
    std::vector<float> vertices = {
        // Front face
        -0.5f,
        -0.5f,
        0.5f,
        0.0f,
        0.0f,
        0.5f,
        -0.5f,
        0.5f,
        1.0f,
        0.0f,
        0.5f,
        0.5f,
        0.5f,
        1.0f,
        1.0f,
        -0.5f,
        0.5f,
        0.5f,
        0.0f,
        1.0f,

        // Back face
        -0.5f,
        -0.5f,
        -0.5f,
        1.0f,
        0.0f,
        0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        0.0f,
        0.5f,
        0.5f,
        -0.5f,
        0.0f,
        1.0f,
        -0.5f,
        0.5f,
        -0.5f,
        1.0f,
        1.0f,
    };

    std::vector<uint32_t> indices = {// Front face
                                     0, 1, 2, 2, 3, 0,
                                     // Back face
                                     4, 5, 6, 6, 7, 4,
                                     // Left face
                                     7, 3, 0, 0, 4, 7,
                                     // Right face
                                     1, 5, 6, 6, 2, 1,
                                     // Top face
                                     3, 2, 6, 6, 7, 3,
                                     // Bottom face
                                     0, 1, 5, 5, 4, 0};

    m_cubeMesh = m_renderer->CreateMesh(vertices, indices);
    if (!m_cubeMesh) {
      std::cerr << "Failed to create cube mesh!" << std::endl;
      return false;
    }

    return true;
  }

  void SetupCamera() {
    // Setup perspective projection
    float fov = AquaVisual::Math::ToRadians(45.0f);
    float aspect = 1024.0f / 768.0f;
    float near = 0.1f;
    float far = 100.0f;

    AquaVisual::Matrix4 projection =
        AquaVisual::Matrix4::Perspective(fov, aspect, near, far);
    m_renderer->SetProjectionMatrix(projection);

    // Setup view matrix (camera looking at origin from distance)
    AquaVisual::Vector3 eye(0, 0, 3);
    AquaVisual::Vector3 center(0, 0, 0);
    AquaVisual::Vector3 up(0, 1, 0);

    AquaVisual::Matrix4 view = AquaVisual::Matrix4::LookAt(eye, center, up);
    m_renderer->SetViewMatrix(view);
  }

  void SetupInput() {
    // Setup key callback for game controls
    m_renderer->SetKeyCallback(
        [this](int key, int scancode, int action, int mods) {
          if (key == 256 && action == 1) { // ESC key pressed
                                           // Could signal game to quit here
          }
          if (key == 32 && action == 1) { // Space key pressed
            m_rotationSpeed = (m_rotationSpeed > 0) ? 0.0f : 1.0f;
          }
        });

    // Setup mouse callback for camera control
    m_renderer->SetMouseCallback([this](double xpos, double ypos) {
      // Could implement camera rotation here
    });

    // Setup resize callback
    m_renderer->SetResizeCallback([this](uint32_t width, uint32_t height) {
      // Update projection matrix for new aspect ratio
      float fov = AquaVisual::Math::ToRadians(45.0f);
      float aspect = static_cast<float>(width) / static_cast<float>(height);
      float near = 0.1f;
      float far = 100.0f;

      AquaVisual::Matrix4 projection =
          AquaVisual::Matrix4::Perspective(fov, aspect, near, far);
      m_renderer->SetProjectionMatrix(projection);
    });
  }

  void Update() {
    // Update game logic
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime =
        std::chrono::duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;

    m_time += deltaTime;

    // Rotate the cube
    m_cubeTransform.rotation.y = m_time * m_rotationSpeed;
    m_cubeTransform.rotation.x = m_time * m_rotationSpeed * 0.5f;
  }

  void Render() {
    // Set clear color
    m_renderer->SetClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    // Begin frame
    m_renderer->BeginFrame();

    // Enable depth testing
    m_renderer->SetDepthTest(true);

    // Draw textured quad (2D overlay)
    m_renderer->DrawTexturedQuad(m_texture, {-100, -100}, {200, 200});

    // Draw rotating cube (3D object)
    m_renderer->DrawMesh(m_cubeMesh, m_cubeTransform);

    // End frame and present
    m_renderer->EndFrame();

    // Print performance info occasionally
    static int frameCount = 0;
    if (++frameCount % 60 == 0) {
      std::cout << "FPS: " << m_renderer->GetFPS()
                << ", Frame Time: " << m_renderer->GetFrameTime() << "ms"
                << std::endl;
    }
  }
};

/**
 * @brief Main entry point
 */
int main() {
  try {
    AquaEngine engine;

    if (!engine.Initialize()) {
      std::cerr << "Failed to initialize AquaEngine!" << std::endl;
      return -1;
    }

    engine.Run();
    engine.Shutdown();

    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return -1;
  }
}