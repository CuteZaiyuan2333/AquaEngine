#include "../../Core/Platform/Application.h"
#include "../../Core/Renderer/Texture.h"
#include "../../Core/Utils/Logger.h"
#include <iostream>
#include <memory>

class TextureShowcaseApp : public Aqua::Platform::Application {
private:
    std::unique_ptr<AquaEngine::Texture> redTexture;
    std::unique_ptr<AquaEngine::Texture> greenTexture;

public:
    bool Initialize() override {
        std::cout << "\n=== AquaEngine Texture Showcase ===" << std::endl;
        std::cout << "Initializing texture demonstration..." << std::endl;
        
        if (!Aqua::Platform::Application::Initialize()) {
            std::cout << "ERROR: Base class initialization failed" << std::endl;
            return false;
        }

        std::cout << "✓ Application initialized successfully" << std::endl;

        auto vulkanContext = GetVulkanContext();
        if (vulkanContext) {
            AquaEngine::Texture::SetVulkanContext(vulkanContext.get());
            std::cout << "✓ VulkanContext set for texture system" << std::endl;
        } else {
            std::cout << "ERROR: VulkanContext is null" << std::endl;
            return false;
        }

        if (!LoadTextures()) {
            std::cout << "ERROR: Failed to load textures" << std::endl;
            return false;
        }

        std::cout << "\n=== Texture Showcase Ready! ===" << std::endl;
        std::cout << "Press ESC to exit" << std::endl;
        std::cout << "=================================" << std::endl;

        return true;
    }

    bool LoadTextures() {
        std::cout << "\nLoading textures..." << std::endl;
        
        redTexture = std::make_unique<AquaEngine::Texture>();
        std::string redTexturePath = "Assets/Textures/test_red_4x4.png";
        if (redTexture->LoadFromFile(redTexturePath)) {
            std::cout << "✓ Red texture loaded: " << redTexturePath << std::endl;
            std::cout << "  Size: " << redTexture->GetWidth() << "x" << redTexture->GetHeight() << std::endl;
        } else {
            std::cout << "✗ Failed to load red texture: " << redTexturePath << std::endl;
            return false;
        }

        greenTexture = std::make_unique<AquaEngine::Texture>();
        std::string greenTexturePath = "Assets/Textures/test_green_8x8.png";
        if (greenTexture->LoadFromFile(greenTexturePath)) {
            std::cout << "✓ Green texture loaded: " << greenTexturePath << std::endl;
            std::cout << "  Size: " << greenTexture->GetWidth() << "x" << greenTexture->GetHeight() << std::endl;
        } else {
            std::cout << "✗ Failed to load green texture: " << greenTexturePath << std::endl;
            return false;
        }

        return true;
    }

    void Update() override {
        // 简单的更新循环
    }

    void Render() override {
        // 简单的渲染循环
    }

    void Cleanup() override {
        std::cout << "\nCleaning up Texture Showcase..." << std::endl;
        
        if (redTexture) {
            redTexture->Cleanup();
            redTexture.reset();
            std::cout << "✓ Red texture cleaned up" << std::endl;
        }

        if (greenTexture) {
            greenTexture->Cleanup();
            greenTexture.reset();
            std::cout << "✓ Green texture cleaned up" << std::endl;
        }

        AquaEngine::Texture::CleanupStaticResources();
        std::cout << "✓ Static texture resources cleaned up" << std::endl;

        Aqua::Platform::Application::Cleanup();
        
        std::cout << "✓ Texture Showcase cleanup complete" << std::endl;
    }
};

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    AquaEngine Texture Showcase" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "This demo showcases texture loading and" << std::endl;
    std::cout << "management capabilities of AquaEngine." << std::endl;
    std::cout << "========================================" << std::endl;

    TextureShowcaseApp app;
    app.Run();

    std::cout << "\nTexture Showcase finished." << std::endl;
    return 0;
}