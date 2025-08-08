#include <iostream>
#include <chrono>
#include <thread>
#include <memory>

#include "../../Core/Platform/Window.h"
#include "../../Core/Renderer/VulkanContext.h"
#include "../../Core/Renderer/Texture.h"
#include "../../Core/Renderer/Renderer.h"
#include "../../Core/Utils/Logger.h"

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    AquaEngine Simple Texture Render" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "This demo shows a textured triangle using the built-in renderer" << std::endl;
    std::cout << "Press ESC or close window to exit" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        // 1. Create window
        std::cout << "\n1. Creating window..." << std::endl;
        AquaEngine::Platform::WindowProperties props("AquaEngine Simple Texture Render", 800, 600);
        auto window = std::make_unique<AquaEngine::Platform::Window>(props);
        std::cout << "OK Window created successfully" << std::endl;

        // 2. Initialize Vulkan context
        std::cout << "\n2. Initializing Vulkan context..." << std::endl;
        auto vulkanContext = std::make_unique<Aqua::Renderer::VulkanContext>();
        if (!vulkanContext->Initialize(window->GetNativeWindow(), 800, 600)) {
            std::cerr << "ERROR: Vulkan context initialization failed" << std::endl;
            return -1;
        }
        std::cout << "OK Vulkan context initialized successfully" << std::endl;

        // 3. Initialize renderer
        std::cout << "\n3. Initializing renderer..." << std::endl;
        Aqua::Renderer::Renderer renderer;
        auto vulkanContextShared = std::shared_ptr<Aqua::Renderer::VulkanContext>(vulkanContext.release());
        renderer.Initialize(vulkanContextShared);
        std::cout << "OK Renderer initialized successfully" << std::endl;

        // 4. Setup texture system
        std::cout << "\n4. Setting up texture system..." << std::endl;
        AquaEngine::Texture::SetVulkanContext(vulkanContextShared.get());
        std::cout << "OK Texture system initialized" << std::endl;

        // 5. Load texture
        std::cout << "\n5. Loading texture..." << std::endl;
        std::string texturePath = "Assets/Textures/test_red_4x4.png";
        std::cout << "Loading texture from: " << texturePath << std::endl;
        
        auto texture = std::make_unique<AquaEngine::Texture>();
        if (!texture->LoadFromFile(texturePath)) {
            std::cerr << "ERROR: Failed to load texture!" << std::endl;
            return -1;
        }
        
        std::cout << "OK Texture loaded successfully!" << std::endl;
        std::cout << "  Size: " << texture->GetWidth() << "x" << texture->GetHeight() << std::endl;

        // 6. Set renderer texture
        std::cout << "\n6. Setting up renderer texture..." << std::endl;
        renderer.SetCurrentTexture(texture->GetImageView(), texture->GetSampler());
        std::cout << "OK Texture set for rendering" << std::endl;

        std::cout << "\n========================================" << std::endl;
        std::cout << "           Rendering Setup Complete" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "OK Window: " << window->GetWidth() << "x" << window->GetHeight() << std::endl;
        std::cout << "OK Vulkan: Initialized" << std::endl;
        std::cout << "OK Texture: " << texture->GetWidth() << "x" << texture->GetHeight() << std::endl;
        std::cout << "OK Renderer: Ready" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Rendering textured triangle..." << std::endl;
        std::cout << "========================================" << std::endl;

        // 7. Render loop
        std::cout << "\n7. Starting render loop..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        int frameCount = 0;
        bool firstFrame = true;

        while (!window->ShouldClose()) {
            window->OnUpdate();
            
            // Check exit conditions
            if (window->ShouldClose()) {
                std::cout << "\nUser closed window, starting cleanup..." << std::endl;
                break;
            }

            // Begin frame
            renderer.BeginFrame();
            
            // Render textured triangle
            renderer.RenderTriangle();
            
            // End frame
            renderer.EndFrame();
            
            frameCount++;
            
            if (firstFrame) {
                std::cout << "First frame rendered with texture!" << std::endl;
                std::cout << "Texture details:" << std::endl;
                std::cout << "  Size: " << texture->GetWidth() << "x" << texture->GetHeight() << std::endl;
                std::cout << "  VkImage: " << (texture->GetImage() ? "Valid" : "Invalid") << std::endl;
                std::cout << "  VkImageView: " << (texture->GetImageView() ? "Valid" : "Invalid") << std::endl;
                std::cout << "  VkSampler: " << (texture->GetSampler() ? "Valid" : "Invalid") << std::endl;
                firstFrame = false;
            }
            
            // Show status every second
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
            
            if (elapsed > 0 && frameCount % 60 == 0) {
                std::cout << "Rendering: " << elapsed << "s, Frame: " << frameCount 
                         << " (Texture: " << texture->GetWidth() << "x" << texture->GetHeight() << ")" << std::endl;
            }
            
            // Limit frame rate (remove or reduce for higher FPS)
            // std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
            // std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Unlimited FPS
        }

        // 8. Cleanup resources
        std::cout << "\n8. Cleaning up resources..." << std::endl;
        
        // Cleanup renderer
        renderer.Cleanup();
        
        // Cleanup texture
        texture->Cleanup();
        texture.reset();
        AquaEngine::Texture::CleanupStaticResources();
        
        // Cleanup Vulkan context
        vulkanContextShared->Cleanup();
        
        std::cout << "OK Cleanup completed" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "ERROR: Exception occurred: " << e.what() << std::endl;
        return -1;
    }

    std::cout << "\nSimple texture render demo finished!" << std::endl;
    return 0;
}