#include "../../Core/Platform/Application.h"
#include "../../Core/Renderer/Texture.h"
#include "../../Core/Utils/Logger.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

class SimpleTextureDemoApp : public Aqua::Platform::Application {
private:
    std::unique_ptr<AquaEngine::Texture> m_texture;
    bool m_shouldExit = false;

public:
    bool Initialize() override {
        AQUA_LOG_INFO("Application", "Initializing Simple Texture Demo...");
        
        if (!Aqua::Platform::Application::Initialize()) {
            AQUA_LOG_ERROR("Application", "Base class initialization failed");
            return false;
        }

        AQUA_LOG_INFO("Application", "Application initialized successfully");

        AquaEngine::Texture::SetVulkanContext(GetVulkanContext().get());
        AQUA_LOG_INFO("Application", "VulkanContext set for texture system");

        LoadTestTexture();

        GetWindow()->SetKeyCallback([this](int key, int action) {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                AQUA_LOG_INFO("Application", "ESC pressed - exiting...");
                m_shouldExit = true;
            }
        });

        AQUA_LOG_INFO("Application", "Simple Texture Demo initialized successfully!");
        AQUA_LOG_INFO("Application", "Press ESC to exit");

        return true;
    }

    void LoadTestTexture() {
        AQUA_LOG_INFO("Application", "Loading test texture...");
        
        m_texture = std::make_unique<AquaEngine::Texture>();
        
        std::string texturePath = "Assets/Textures/test_red_4x4.png";
        if (m_texture->LoadFromFile(texturePath)) {
            AQUA_LOG_INFO("Application", "Texture loaded successfully: " + texturePath);
            AQUA_LOG_INFO("Application", "Texture size: " + 
                std::to_string(m_texture->GetWidth()) + "x" + 
                std::to_string(m_texture->GetHeight()));
        } else {
            AQUA_LOG_ERROR("Application", "Failed to load texture: " + texturePath);
            texturePath = "Assets/Textures/test_green_8x8.png";
            if (m_texture->LoadFromFile(texturePath)) {
                AQUA_LOG_INFO("Application", "Backup texture loaded: " + texturePath);
            } else {
                AQUA_LOG_ERROR("Application", "Failed to load backup texture");
            }
        }
    }

    void Update() override {
        if (m_shouldExit) {
            GetWindow()->SetShouldClose(true);
        }
    }

    void Render() override {
        // 简单的渲染循环
    }

    void Cleanup() override {
        AQUA_LOG_INFO("Application", "Cleaning up Simple Texture Demo...");
        
        if (m_texture) {
            m_texture->Cleanup();
            m_texture.reset();
        }

        AquaEngine::Texture::CleanupStaticResources();
        Aqua::Platform::Application::Cleanup();
        
        AQUA_LOG_INFO("Application", "Simple Texture Demo cleanup complete");
    }
};

int main() {
    std::cout << "=== AquaEngine Simple Texture Demo ===" << std::endl;
    std::cout << "This demo tests basic texture loading functionality." << std::endl;
    std::cout << "=======================================" << std::endl;

    SimpleTextureDemoApp app;
    app.Run();

    std::cout << "Simple Texture Demo finished." << std::endl;
    return 0;
}