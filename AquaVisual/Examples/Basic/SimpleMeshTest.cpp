#include "AquaVisual/AquaVisual.h"
#include "AquaVisual/Core/Camera.h"
#include "AquaVisual/Core/VulkanRenderer.h"
#include "AquaVisual/Resources/Mesh.h"
#include <iostream>
#include <memory>

using namespace AquaVisual;

int main() {
  std::cout << "Starting Simple Mesh Test..." << std::endl;

  try {
    // Initialize AquaVisual
    if (!AquaVisual::Initialize()) {
      std::cerr << "Failed to initialize AquaVisual" << std::endl;
      return -1;
    }

    // Create renderer configuration
    RendererConfig config;
    config.width = 800;
    config.height = 600;
    config.title = "Simple Mesh Test";
    config.enableValidation = true;
    config.enableVSync = true;

    // Create VulkanRenderer
    auto renderer = std::make_unique<VulkanRenderer>();
    renderer->SetConfig(config);

    if (!renderer->Initialize()) {
      std::cerr << "Failed to initialize renderer" << std::endl;
      AquaVisual::Shutdown();
      return -1;
    }

    std::cout << "Renderer initialized successfully" << std::endl;

    // Create camera
    Camera camera;
    camera.SetPosition(Vec3(0, 0, 3));
    camera.SetTarget(Vec3(0, 0, 0));
    camera.SetUp(Vec3(0, 1, 0));
    camera.SetPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

    // Create a simple cube mesh
    auto cubeMesh = Mesh::CreateCube(1.0f);
    if (!cubeMesh) {
      std::cerr << "Failed to create cube mesh" << std::endl;
      renderer->Shutdown();
      AquaVisual::Shutdown();
      return -1;
    }

    std::cout << "Created cube mesh with " << cubeMesh->GetVertexCount()
              << " vertices and " << cubeMesh->GetIndexCount() << " indices"
              << std::endl;

    // Test the RenderMesh method
    std::cout << "Testing RenderMesh method..." << std::endl;

    // Set camera
    renderer->SetCamera(camera);

    // Begin frame
    if (renderer->BeginFrame()) {
      // Clear screen
      renderer->Clear(0.2f, 0.3f, 0.3f, 1.0f);

      // Render the cube mesh
      renderer->RenderMesh(*cubeMesh);

      // End frame
      renderer->EndFrame();

      std::cout << "Successfully rendered one frame with mesh" << std::endl;
    } else {
      std::cout << "Failed to begin frame" << std::endl;
    }

    // Cleanup
    renderer->Shutdown();
    AquaVisual::Shutdown();

    std::cout << "Simple Mesh Test completed successfully!" << std::endl;
    return 0;

  } catch (const std::exception &e) {
    std::cerr << "Exception caught: " << e.what() << std::endl;
    AquaVisual::Shutdown();
    return -1;
  }
}