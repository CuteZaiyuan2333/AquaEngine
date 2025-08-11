#include "AquaVisual/AquaVisualMVP.h"
#include <iostream>
#include <memory>
#include <chrono>

using namespace AquaVisual;

int main() {
    std::cout << "Starting DualCubeDemo..." << std::endl;
    std::cout << "AquaVisual MVP Version: " << GetMVPVersion() << std::endl;

    // Initialize AquaVisual
    if (!InitializeMVP()) {
        std::cerr << "Failed to initialize AquaVisual MVP" << std::endl;
        return -1;
    }

    try {
        // Create renderer configuration
        RendererConfig config;
        config.width = 1200;  // 增加宽度以容纳两个立方体
        config.height = 600;
        config.title = "DualCubeDemo - AquaVisual MVP";
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
        Vector3 cameraPos(0.0f, 0.0f, 8.0f);  // 稍微远一点以看到两个立方体
        Vector3 cameraTarget(0.0f, 0.0f, 0.0f);
        Vector3 cameraUp(0.0f, 1.0f, 0.0f);
        
        camera.SetPosition(cameraPos);
        camera.SetTarget(cameraTarget);
        camera.SetUp(cameraUp);
        camera.SetPerspective(45.0f, 1200.0f / 600.0f, 0.1f, 100.0f);

        // Create dual cube mesh
        std::cout << "Creating dual cube mesh..." << std::endl;
        float cubeSize = 2.0f;
        
        // 创建包含两个立方体的网格数据
        auto singleCube = Mesh::CreateCube(cubeSize);
        if (!singleCube) {
            std::cerr << "Failed to create single cube mesh" << std::endl;
            return -1;
        }
        
        // 获取单个立方体的顶点和索引数据
        const auto& singleVertices = singleCube->GetVertices();
        const auto& singleIndices = singleCube->GetIndices();
        
        // 创建双立方体的顶点和索引数据
        std::vector<Vertex> dualVertices;
        std::vector<uint32_t> dualIndices;
        
        // 添加第一个立方体的数据（原始位置）
        dualVertices.insert(dualVertices.end(), singleVertices.begin(), singleVertices.end());
        dualIndices.insert(dualIndices.end(), singleIndices.begin(), singleIndices.end());
        
        // 添加第二个立方体的数据（索引需要偏移）
        uint32_t vertexOffset = static_cast<uint32_t>(singleVertices.size());
        dualVertices.insert(dualVertices.end(), singleVertices.begin(), singleVertices.end());
        
        for (uint32_t index : singleIndices) {
            dualIndices.push_back(index + vertexOffset);
        }
        
        // 创建双立方体网格
        auto dualCubeMesh = std::make_unique<Mesh>(dualVertices, dualIndices);
        
        std::cout << "Dual cube mesh created successfully with " 
                  << dualCubeMesh->GetVertexCount() << " vertices and " 
                  << dualCubeMesh->GetIndexCount() << " indices" << std::endl;

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
                continue; // Skip this frame if begin failed
            }

            // Set camera
            renderer->SetCamera(camera);

            // Clear screen with dark blue color
            renderer->Clear(0.1f, 0.1f, 0.1f, 1.0f);

            // 渲染双立方体
            renderer->RenderMesh(*dualCubeMesh);

            // End frame
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
    
    std::cout << "DualCubeDemo completed successfully!" << std::endl;
    return 0;
}