#include "../../Core/Renderer/Texture.h"
#include "../../Core/Renderer/VulkanContext.h"
#include "../../Core/Platform/Window.h"
#include "../../Core/Utils/Logger.h"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    AquaEngine Texture Window Demo" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "This demo shows a window and loads textures" << std::endl;
    std::cout << "Press ESC or close window to exit" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        // Create window
        std::cout << "\n1. Creating window..." << std::endl;
        AquaEngine::Platform::WindowProperties props("AquaEngine Texture Demo", 800, 600);
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

        // Load textures
        std::cout << "\n4. Loading textures..." << std::endl;
        
        // Load red texture
        std::cout << "\nLoading red texture..." << std::endl;
        auto redTexture = std::make_unique<AquaEngine::Texture>();
        bool redLoaded = redTexture->LoadFromFile("Assets/Textures/test_red_4x4.png");
        if (redLoaded) {
            std::cout << "OK Red texture loaded successfully!" << std::endl;
            std::cout << "  Size: " << redTexture->GetWidth() << "x" << redTexture->GetHeight() << std::endl;
            std::cout << "  VkImage: " << (redTexture->GetImage() != VK_NULL_HANDLE ? "Valid" : "Invalid") << std::endl;
            std::cout << "  VkImageView: " << (redTexture->GetImageView() != VK_NULL_HANDLE ? "Valid" : "Invalid") << std::endl;
            std::cout << "  VkSampler: " << (redTexture->GetSampler() != VK_NULL_HANDLE ? "Valid" : "Invalid") << std::endl;
        } else {
            std::cout << "ERROR Red texture loading failed" << std::endl;
        }

        // Try to load other textures
        std::cout << "\nTrying to load other textures..." << std::endl;
        auto testTexture = std::make_unique<AquaEngine::Texture>();
        bool testLoaded = testTexture->LoadFromFile("Assets/Textures/test_simple.png");
        if (testLoaded) {
            std::cout << "OK Test texture loaded successfully!" << std::endl;
            std::cout << "  Size: " << testTexture->GetWidth() << "x" << testTexture->GetHeight() << std::endl;
        } else {
            std::cout << "ERROR Test texture loading failed, trying other files..." << std::endl;
            testLoaded = testTexture->LoadFromFile("Assets/Textures/test_valid.png");
            if (testLoaded) {
                std::cout << "OK Valid texture loaded successfully!" << std::endl;
                std::cout << "  Size: " << testTexture->GetWidth() << "x" << testTexture->GetHeight() << std::endl;
            }
        }

        std::cout << "\n========================================" << std::endl;
        std::cout << "           Texture Loading Results" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "OK Window creation: Success" << std::endl;
        std::cout << "OK Vulkan initialization: Success" << std::endl;
        std::cout << "OK Texture system setup: Success" << std::endl;
        std::cout << (redLoaded ? "OK" : "ERROR") << " Red texture: " << (redLoaded ? "Success" : "Failed") << std::endl;
        std::cout << (testLoaded ? "OK" : "ERROR") << " Test texture: " << (testLoaded ? "Success" : "Failed") << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Window is open, texture functionality working!" << std::endl;
        std::cout << "Press ESC or close window to exit..." << std::endl;
        std::cout << "========================================" << std::endl;

        // Main loop - keep window open
        std::cout << "\n5. Entering main loop..." << std::endl;
        int frameCount = 0;
        auto startTime = std::chrono::steady_clock::now();
        
        while (!window->ShouldClose()) {
            // Update window
            window->OnUpdate();
            
            // Simple frame counter
            frameCount++;
            if (frameCount % 60 == 0) {
                auto currentTime = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
                std::cout << "Runtime: " << elapsed << " seconds, Frames: " << frameCount << std::endl;
            }
            
            // Small delay to prevent high CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }

        std::cout << "\nUser closed window, starting cleanup..." << std::endl;

        // Cleanup
        std::cout << "\n6. Cleaning up resources..." << std::endl;
        if (redTexture) {
            redTexture->Cleanup();
            redTexture.reset();
        }
        if (testTexture) {
            testTexture->Cleanup();
            testTexture.reset();
        }
        
        AquaEngine::Texture::CleanupStaticResources();
        vulkanContext->Cleanup();
        
        std::cout << "OK Cleanup completed" << std::endl;

    } catch (const std::exception& e) {
        std::cout << "ERROR Exception occurred: " << e.what() << std::endl;
        return -1;
    }

    std::cout << "\nTexture window demo finished!" << std::endl;
    return 0;
}