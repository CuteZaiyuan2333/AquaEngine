#include <AquaVisual/AquaVisualMVP.h>
#include <chrono>
#include <iostream>
#include <cmath>

using namespace AquaVisual;

int main() {
    std::cout << "=== AquaVisual Cube Window Demo ===" << std::endl;
    std::cout << "Version: " << AQUAVISUAL_MVP_VERSION << std::endl;

    // 1. Initialize AquaVisual MVP
    if (!InitializeMVP()) {
        std::cerr << "Failed to initialize AquaVisual MVP!" << std::endl;
        return -1;
    }
    std::cout << "AquaVisual MVP initialized successfully!" << std::endl;

    // 2. Create renderer configuration
    RendererConfig config;
    config.width = 800;
    config.height = 600;
    config.title = "AquaVisual Cube Demo - Real Window";
    config.enableValidation = true;
    config.enableVSync = true;

    // 3. Create renderer (already initialized in Create method)
    auto renderer = Renderer::Create(config);
    if (!renderer) {
        std::cerr << "Failed to create renderer!" << std::endl;
        ShutdownMVP();
        return -1;
    }

    std::cout << "Renderer created and initialized successfully!" << std::endl;

    // 4. Create camera
    Camera camera;
    camera.SetPosition(Vec3(3.0f, 3.0f, 3.0f));
    camera.SetTarget(Vec3(0.0f, 0.0f, 0.0f));
    camera.SetFOV(45.0f);
    camera.SetAspectRatio(800.0f / 600.0f);

    renderer->SetCamera(camera);
    std::cout << "Camera configured!" << std::endl;

    // 5. Create cube mesh
    auto cube = Primitives::CreateCube(2.0f);
    if (!cube) {
        std::cerr << "Failed to create cube mesh!" << std::endl;
        renderer->Shutdown();
        ShutdownMVP();
        return -1;
    }

    std::cout << "Cube mesh created with " << cube->GetVertexCount() 
              << " vertices and " << cube->GetIndexCount() << " indices" << std::endl;

    // 6. Main render loop
    std::cout << "Starting render loop..." << std::endl;
    std::cout << "Press ESC or close window to exit" << std::endl;

    auto startTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    float rotation = 0.0f;

    while (!renderer->ShouldClose()) {
        // Process events
        renderer->PollEvents();

        // Update rotation
        rotation += 0.01f;
        if (rotation > 2.0f * 3.14159f) {
            rotation = 0.0f;
        }

        // Update camera position for rotation effect
        float radius = 5.0f;
        float x = radius * cos(rotation);
        float z = radius * sin(rotation);
        camera.SetPosition(Vec3(x, 3.0f, z));
        camera.SetTarget(Vec3(0.0f, 0.0f, 0.0f));
        renderer->SetCamera(camera);

        // Begin frame
        if (renderer->BeginFrame()) {
            // Clear screen with dark blue color
            renderer->Clear(0.1f, 0.1f, 0.3f, 1.0f);

            // Render the cube
            renderer->RenderMesh(*cube);

            // End frame
            renderer->EndFrame();
        }

        frameCount++;

        // Output FPS every second
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            currentTime - startTime);
        if (elapsed.count() >= 1) {
            std::cout << "FPS: " << frameCount << " | Rotation: " 
                      << (rotation * 180.0f / 3.14159f) << " degrees" << std::endl;
            frameCount = 0;
            startTime = currentTime;
        }
    }

    std::cout << "Render loop ended" << std::endl;

    // 7. Cleanup
    renderer->Shutdown();
    ShutdownMVP();
    std::cout << "Cube demo completed successfully!" << std::endl;

    return 0;
}