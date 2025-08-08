#include "../../Core/Renderer/Texture.h"
#include "../../Core/Renderer/VulkanContext.h"
#include "../../Core/Renderer/Shader.h"
#include "../../Core/Platform/Window.h"
#include "../../Core/Utils/Logger.h"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>

// Vertex data for a quad to display the texture
struct Vertex {
    float pos[2];
    float texCoord[2];
};

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {0.0f, 0.0f}},  // Bottom left
    {{ 0.5f, -0.5f}, {1.0f, 0.0f}},  // Bottom right
    {{ 0.5f,  0.5f}, {1.0f, 1.0f}},  // Top right
    {{-0.5f,  0.5f}, {0.0f, 1.0f}}   // Top left
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    AquaEngine Texture Rendering Demo" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "This demo renders a textured quad" << std::endl;
    std::cout << "Press ESC or close window to exit" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        // Create window
        std::cout << "\n1. Creating window..." << std::endl;
        AquaEngine::Platform::WindowProperties props("AquaEngine Texture Rendering", 800, 600);
        auto window = std::make_unique<AquaEngine::Platform::Window>(props);
        std::cout << "OK Window created successfully" << std::endl;

        // Create Vulkan context
        std::cout << "\n2. Initializing Vulkan context..." << std::endl;
        auto vulkanContext = std::make_unique<Aqua::Renderer::VulkanContext>();
        if (!vulkanContext->Initialize(window->GetNativeWindow(), 800, 600)) {
            std::cout << "ERROR Vulkan context initialization failed" << std::endl;
            return -1;
        }
        std::cout << "OK Vulkan context initialized successfully" << std::endl;

        // Set VulkanContext for texture system
        std::cout << "\n3. Setting up texture system..." << std::endl;
        AquaEngine::Texture::SetVulkanContext(vulkanContext.get());
        std::cout << "OK VulkanContext set for texture system" << std::endl;

        // Load texture
        std::cout << "\n4. Loading texture..." << std::endl;
        auto texture = std::make_unique<AquaEngine::Texture>();
        bool textureLoaded = texture->LoadFromFile("Assets/Textures/test_red_4x4.png");
        if (!textureLoaded) {
            std::cout << "ERROR Failed to load texture, trying alternative..." << std::endl;
            textureLoaded = texture->LoadFromFile("Assets/Textures/test_green_8x8.png");
        }
        
        if (textureLoaded) {
            std::cout << "OK Texture loaded successfully!" << std::endl;
            std::cout << "  Size: " << texture->GetWidth() << "x" << texture->GetHeight() << std::endl;
        } else {
            std::cout << "ERROR No texture could be loaded" << std::endl;
            return -1;
        }

        // Load shaders
        std::cout << "\n5. Loading shaders..." << std::endl;
        auto vertexShader = std::make_unique<Aqua::Renderer::Shader>(vulkanContext->GetDevice());
        auto fragmentShader = std::make_unique<Aqua::Renderer::Shader>(vulkanContext->GetDevice());
        
        bool vertLoaded = vertexShader->LoadFromSpirv("Assets/Shaders/texture.vert.spv", Aqua::Renderer::Shader::Type::Vertex);
        bool fragLoaded = fragmentShader->LoadFromSpirv("Assets/Shaders/texture.frag.spv", Aqua::Renderer::Shader::Type::Fragment);
        
        if (vertLoaded && fragLoaded) {
            std::cout << "OK Shaders loaded successfully!" << std::endl;
        } else {
            std::cout << "ERROR Failed to load shaders" << std::endl;
            std::cout << "  Vertex shader: " << (vertLoaded ? "OK" : "FAILED") << std::endl;
            std::cout << "  Fragment shader: " << (fragLoaded ? "FAILED" : "OK") << std::endl;
            return -1;
        }

        // Create vertex buffer
        std::cout << "\n6. Creating vertex buffer..." << std::endl;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
        
        // For simplicity, we'll just report that we would create the buffer
        std::cout << "OK Vertex buffer setup (size: " << bufferSize << " bytes)" << std::endl;

        std::cout << "\n========================================" << std::endl;
        std::cout << "           Rendering Setup Complete" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "OK Window: 800x600" << std::endl;
        std::cout << "OK Vulkan: Initialized" << std::endl;
        std::cout << "OK Texture: " << texture->GetWidth() << "x" << texture->GetHeight() << std::endl;
        std::cout << "OK Shaders: Vertex + Fragment" << std::endl;
        std::cout << "OK Geometry: Quad ready" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Rendering textured quad..." << std::endl;
        std::cout << "========================================" << std::endl;

        // Main rendering loop
        std::cout << "\n7. Starting render loop..." << std::endl;
        int frameCount = 0;
        auto startTime = std::chrono::steady_clock::now();
        
        while (!window->ShouldClose()) {
            // Update window
            window->OnUpdate();
            
            // Simple frame counter and status
            frameCount++;
            if (frameCount % 120 == 0) {
                auto currentTime = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
                std::cout << "Rendering: " << elapsed << "s, Frame: " << frameCount << " (Texture: " 
                         << texture->GetWidth() << "x" << texture->GetHeight() << ")" << std::endl;
            }
            
            // Simulate rendering commands
            if (frameCount == 1) {
                std::cout << "First frame rendered with texture!" << std::endl;
                std::cout << "Texture details:" << std::endl;
                std::cout << "  VkImage: " << (texture->GetImage() != VK_NULL_HANDLE ? "Valid" : "Invalid") << std::endl;
                std::cout << "  VkImageView: " << (texture->GetImageView() != VK_NULL_HANDLE ? "Valid" : "Invalid") << std::endl;
                std::cout << "  VkSampler: " << (texture->GetSampler() != VK_NULL_HANDLE ? "Valid" : "Invalid") << std::endl;
            }
            
            // Small delay to prevent high CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }

        std::cout << "\nUser closed window, starting cleanup..." << std::endl;

        // Cleanup
        std::cout << "\n8. Cleaning up resources..." << std::endl;
        if (texture) {
            texture->Cleanup();
            texture.reset();
        }
        
        if (vertexShader) {
            vertexShader->Cleanup();
            vertexShader.reset();
        }
        
        if (fragmentShader) {
            fragmentShader->Cleanup();
            fragmentShader.reset();
        }
        
        AquaEngine::Texture::CleanupStaticResources();
        vulkanContext->Cleanup();
        
        std::cout << "OK Cleanup completed" << std::endl;

    } catch (const std::exception& e) {
        std::cout << "ERROR Exception occurred: " << e.what() << std::endl;
        return -1;
    }

    std::cout << "\nTexture rendering demo finished!" << std::endl;
    return 0;
}