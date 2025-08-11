#include "AquaVisual/AquaVisualMVP.h"
#include <iostream>
#include <memory>
#include <chrono>

using namespace AquaVisual;

int main() {
    std::cout << "Starting DualCubeTexturedDemo...\n";
    std::cout << "AquaVisual MVP Version: " << GetMVPVersion() << '\n';

    // Initialize AquaVisual
    if (!InitializeMVP()) {
        std::cerr << "Failed to initialize AquaVisual MVP\n";
        return -1;
    }

    try {
        // Create renderer configuration
        RendererConfig config;
        config.width = 1200;  // Increase width to accommodate two cubes
        config.height = 600;
        config.title = "DualCubeTexturedDemo - AquaVisual MVP";
        config.enableValidation = true;
        config.enableVSync = true;

        std::cout << "Creating renderer...\n";
        auto renderer = Renderer::Create(config);
        if (!renderer) {
            std::cerr << "Failed to create renderer\n";
            return -1;
        }

        std::cout << "Initializing renderer...\n";
        if (!renderer->Initialize()) {
            std::cerr << "Failed to initialize renderer\n";
            return -1;
        }

        // Load texture
        std::cout << "Loading texture...\n";
        auto texture = Texture::CreateFromFile("AquaVisual/Images/CuteCatImage256x256.bmp");
        if (!texture) {
            std::cerr << "Failed to load texture\n";
            return -1;
        }
        std::cout << "Texture loaded successfully\n";

        // Create camera
        std::cout << "Setting up camera...\n";
        Camera camera;
        Vector3 cameraPos(0.0f, 0.0f, 8.0f);  // Move back a bit to see both cubes
        Vector3 cameraTarget(0.0f, 0.0f, 0.0f);
        Vector3 cameraUp(0.0f, 1.0f, 0.0f);
        
        camera.SetPosition(cameraPos);
        camera.SetTarget(cameraTarget);
        camera.SetUp(cameraUp);
        camera.SetPerspective(45.0f, 1200.0f / 600.0f, 0.1f, 100.0f);

        // Create dual cube mesh
        std::cout << "Creating dual cube mesh...\n";
        float cubeSize = 2.0f;
        
        // Create mesh data containing two cubes
        auto singleCube = Mesh::CreateCube(cubeSize);
        if (!singleCube) {
            std::cerr << "Failed to create single cube mesh\n";
            return -1;
        }
        
        // Get vertex and index data from single cube
        const auto& singleVertices = singleCube->GetVertices();
        const auto& singleIndices = singleCube->GetIndices();
        
        // Create dual cube vertex and index data
        std::vector<Vertex> dualVertices;
        std::vector<uint32_t> dualIndices;
        
        // Add first cube data (left position)
        for (const auto& vertex : singleVertices) {
            Vertex leftVertex = vertex;
            leftVertex.position.x -= 2.5f; // Move left
            dualVertices.push_back(leftVertex);
        }
        dualIndices.insert(dualIndices.end(), singleIndices.begin(), singleIndices.end());
        
        // Add second cube data (right position, indices need offset)
        uint32_t vertexOffset = static_cast<uint32_t>(singleVertices.size());
        for (const auto& vertex : singleVertices) {
            Vertex rightVertex = vertex;
            rightVertex.position.x += 2.5f; // Move right
            dualVertices.push_back(rightVertex);
        }
        
        for (uint32_t index : singleIndices) {
            dualIndices.push_back(index + vertexOffset);
        }
        
        // Create dual cube mesh
        auto dualCubeMesh = std::make_unique<Mesh>(dualVertices, dualIndices);
        
        std::cout << "Dual cube mesh created successfully with " 
                  << dualCubeMesh->GetVertexCount() << " vertices and " 
                  << dualCubeMesh->GetIndexCount() << " indices\n";

        // Set frame rate limit
        renderer->SetFrameRateLimit(FrameRateMode::FPS_60);

        std::cout << "Starting render loop...\n";
        
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
                continue; // Skip this frame if begin failed
            }

            // Set camera
            renderer->SetCamera(camera);

            // Clear screen with dark blue color
            renderer->Clear(0.1f, 0.1f, 0.1f, 1.0f);

            // Render dual cube (pass texture as second parameter)
            renderer->RenderMesh(*dualCubeMesh, texture.get());

            // End frame
            renderer->EndFrame();

            frameCount++;
            
            // Print progress every 60 frames
            if (frameCount % 60 == 0) {
                auto currentTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
                std::cout << "Rendered " << frameCount << " frames in " << duration.count() << " seconds\n";
            }
        }

        std::cout << "Shutting down renderer...\n";
        renderer->Shutdown();

    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << '\n';
        ShutdownMVP();
        return -1;
    }

    std::cout << "Shutting down AquaVisual...\n";
    ShutdownMVP();
    
    std::cout << "DualCubeTexturedDemo completed successfully!\n";
    return 0;
}