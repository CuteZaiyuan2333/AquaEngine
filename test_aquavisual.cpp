#include "AquaVisual/Source/Core/Renderer.h"
#include "AquaVisual/Source/Core/Camera.h"
#include "AquaVisual/Source/Core/Mesh.h"
#include <iostream>
#include <memory>

int main() {
    try {
        std::cout << "Testing AquaVisual Library..." << std::endl;
        
        // Test renderer creation
        auto renderer = AquaVisual::Renderer::Create(AquaVisual::RendererAPI::Vulkan);
        if (!renderer) {
            std::cout << "Failed to create renderer" << std::endl;
            return -1;
        }
        std::cout << "✓ Renderer created successfully" << std::endl;
        
        // Test camera creation
        AquaVisual::Camera camera;
        camera.SetPosition({0.0f, 0.0f, 5.0f});
        camera.SetTarget({0.0f, 0.0f, 0.0f});
        camera.SetUp({0.0f, 1.0f, 0.0f});
        camera.SetPerspective(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
        std::cout << "✓ Camera configured successfully" << std::endl;
        
        // Test mesh creation
        std::vector<AquaVisual::Vertex> vertices = {
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{ 0.0f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}}
        };
        
        std::vector<uint32_t> indices = {0, 1, 2};
        
        auto mesh = std::make_unique<AquaVisual::Mesh>(vertices, indices);
        std::cout << "✓ Mesh created successfully" << std::endl;
        
        std::cout << "All tests passed! AquaVisual library is working correctly." << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}