#include "AquaVisual/AquaVisualMVP.h"
#include <iostream>
#include <memory>
#include <chrono>

using namespace AquaVisual;

int main() {
    std::cout << "Starting TexturedCubeDemo..." << std::endl;
    std::cout << "AquaVisual MVP Version: " << GetMVPVersion() << std::endl;

    // Initialize AquaVisual
    if (!InitializeMVP()) {
        std::cerr << "Failed to initialize AquaVisual MVP" << std::endl;
        return -1;
    }

    try {
        // Create renderer configuration
        RendererConfig config;
        config.width = 800;
        config.height = 600;
        config.title = "TexturedCubeDemo - AquaVisual MVP";
        config.enableValidation = true;
        config.enableVSync = true;

        std::cout << "Creating renderer..." << std::endl;
        auto renderer = Renderer::Create(config);
        if (!renderer) {
            std::cerr << "Failed to create renderer" << std::endl;
            return -1;
        }

        std::cout << "Initializing renderer..." << std::endl;
        if (!renderer->Initialize()) {
            std::cerr << "Failed to initialize renderer" << std::endl;
            return -1;
        }

        // Create camera
        std::cout << "Setting up camera..." << std::endl;
        Camera camera;
        Vector3 cameraPos(0.0f, 0.0f, 5.0f);
        Vector3 cameraTarget(0.0f, 0.0f, 0.0f);
        Vector3 cameraUp(0.0f, 1.0f, 0.0f);
        
        std::cout << "Camera position: (" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")" << std::endl;
        std::cout << "Camera target: (" << cameraTarget.x << ", " << cameraTarget.y << ", " << cameraTarget.z << ")" << std::endl;
        std::cout << "Camera up: (" << cameraUp.x << ", " << cameraUp.y << ", " << cameraUp.z << ")" << std::endl;
        
        camera.SetPosition(cameraPos);
        camera.SetTarget(cameraTarget);
        camera.SetUp(cameraUp);
        camera.SetPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
        
        std::cout << "Camera FOV: 45 degrees, Aspect: " << (800.0f / 600.0f) << ", Near: 0.1, Far: 100.0" << std::endl;

        // Create cube mesh
        std::cout << "Creating cube mesh..." << std::endl;
        float cubeSize = 2.0f;
        std::cout << "Cube size: " << cubeSize << std::endl;
        auto cubeMesh = Mesh::CreateCube(cubeSize);
        if (!cubeMesh) {
            std::cerr << "Failed to create cube mesh" << std::endl;
            return -1;
        }

        std::cout << "Cube mesh created with " << cubeMesh->GetVertexCount() 
                  << " vertices and " << cubeMesh->GetIndexCount() << " indices" << std::endl;
                  
        // Print first few vertices for debugging
        const auto& vertices = cubeMesh->GetVertices();
        std::cout << "First 4 vertices:" << std::endl;
        for (int i = 0; i < std::min(4, (int)vertices.size()); i++) {
            const auto& v = vertices[i];
            std::cout << "  Vertex " << i << ": pos(" << v.position.x << ", " << v.position.y << ", " << v.position.z 
                      << "), normal(" << v.normal.x << ", " << v.normal.y << ", " << v.normal.z << ")" << std::endl;
        }

        // Set frame rate limit
        renderer->SetFrameRateLimit(FrameRateMode::FPS_60);

        std::cout << "Starting render loop..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        int frameCount = 0;
        
        // Main render loop
        while (!renderer->ShouldClose()) {
            renderer->PollEvents();

            // Update camera aspect ratio if window was resized
            uint32_t width, height;
            renderer->GetWindowSize(width, height);
            if (width > 0 && height > 0) {
                camera.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
            }

            // Begin frame
            if (!renderer->BeginFrame()) {
                std::cout << "BeginFrame failed, skipping frame" << std::endl;
                continue; // Skip this frame if begin failed
            }

            // Set camera
            if (frameCount < 5) { // Only print for first few frames
                std::cout << "Frame " << frameCount << ": Setting camera..." << std::endl;
            }
            renderer->SetCamera(camera);

            // Clear screen with dark blue color
            if (frameCount < 5) {
                std::cout << "Frame " << frameCount << ": Clearing screen..." << std::endl;
            }
            renderer->Clear(0.1f, 0.1f, 0.3f, 1.0f);

            // Render the cube
            if (frameCount < 5) {
                std::cout << "Frame " << frameCount << ": Rendering cube..." << std::endl;
            }
            renderer->RenderMesh(*cubeMesh);

            // End frame
            if (frameCount < 5) {
                std::cout << "Frame " << frameCount << ": Ending frame..." << std::endl;
            }
            renderer->EndFrame();

            frameCount++;
            
            // Print progress every 60 frames
            if (frameCount % 60 == 0) {
                auto currentTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
                std::cout << "Rendered " << frameCount << " frames in " << duration.count() << " seconds" << std::endl;
            }
        }

        std::cout << "Shutting down renderer..." << std::endl;
        renderer->Shutdown();

    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        ShutdownMVP();
        return -1;
    }

    std::cout << "Shutting down AquaVisual..." << std::endl;
    ShutdownMVP();
    
    std::cout << "Demo completed successfully!" << std::endl;
    return 0;
}