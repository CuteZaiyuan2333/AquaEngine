#include "../../Core/Platform/Application.h"
#include "../../Core/Renderer/Texture.h"
#include "../../Core/Renderer/Shader.h"
#include "../../Core/Utils/Logger.h"
#include <iostream>
#include <memory>
#include <vulkan/vulkan.h>

class TextureRenderDemoApp : public Aqua::Platform::Application {
private:
    std::unique_ptr<AquaEngine::Texture> m_texture;
    std::unique_ptr<Aqua::Renderer::Shader> m_vertexShader;
    std::unique_ptr<Aqua::Renderer::Shader> m_fragmentShader;

public:
    bool Initialize() override {
        std::cout << "\n=== AquaEngine Texture Render Demo ===" << std::endl;
        std::cout << "Initializing texture render demo..." << std::endl;
        
        if (!Aqua::Platform::Application::Initialize()) {
            std::cout << "ERROR: Base application initialization failed" << std::endl;
            return false;
        }

        std::cout << "✓ Application initialized successfully" << std::endl;

        auto vulkanContext = GetVulkanContext();
        if (!vulkanContext) {
            std::cout << "ERROR: VulkanContext is null" << std::endl;
            return false;
        }

        // Set VulkanContext for texture system
        AquaEngine::Texture::SetVulkanContext(vulkanContext.get());
        std::cout << "✓ VulkanContext set for texture system" << std::endl;

        // Load texture
        if (!LoadTexture()) {
            std::cout << "ERROR: Texture loading failed" << std::endl;
            return false;
        }

        // Load shaders
        if (!LoadShaders()) {
            std::cout << "ERROR: Shader loading failed" << std::endl;
            return false;
        }

        std::cout << "\n=== Texture Render Demo Ready! ===" << std::endl;
        std::cout << "Press ESC to exit" << std::endl;
        std::cout << "=================================" << std::endl;

        return true;
    }

    bool LoadTexture() {
        std::cout << "\nLoading texture..." << std::endl;
        
        m_texture = std::make_unique<AquaEngine::Texture>();
        std::string texturePath = "Assets/Textures/test_red_4x4.png";
        
        if (m_texture->LoadFromFile(texturePath)) {
            std::cout << "✓ Texture loaded successfully: " << texturePath << std::endl;
            std::cout << "  Size: " << m_texture->GetWidth() << "x" << m_texture->GetHeight() << std::endl;
            std::cout << "  VkImage: " << (m_texture->GetImage() != VK_NULL_HANDLE ? "Valid" : "Invalid") << std::endl;
            std::cout << "  VkImageView: " << (m_texture->GetImageView() != VK_NULL_HANDLE ? "Valid" : "Invalid") << std::endl;
            std::cout << "  VkSampler: " << (m_texture->GetSampler() != VK_NULL_HANDLE ? "Valid" : "Invalid") << std::endl;
            return true;
        } else {
            std::cout << "✗ Texture loading failed: " << texturePath << std::endl;
            return false;
        }
    }

    bool LoadShaders() {
        std::cout << "\nLoading shaders..." << std::endl;
        
        auto vulkanContext = GetVulkanContext();
        
        m_vertexShader = std::make_unique<Aqua::Renderer::Shader>(vulkanContext->GetDevice());
        if (!m_vertexShader->LoadFromSpirv("Assets/Shaders/texture.vert.spv", Aqua::Renderer::Shader::Type::Vertex)) {
            std::cout << "✗ Vertex shader loading failed" << std::endl;
            return false;
        }
        std::cout << "✓ Vertex shader loaded successfully" << std::endl;

        m_fragmentShader = std::make_unique<Aqua::Renderer::Shader>(vulkanContext->GetDevice());
        if (!m_fragmentShader->LoadFromSpirv("Assets/Shaders/texture.frag.spv", Aqua::Renderer::Shader::Type::Fragment)) {
            std::cout << "✗ Fragment shader loading failed" << std::endl;
            return false;
        }
        std::cout << "✓ Fragment shader loaded successfully" << std::endl;

        return true;
    }

    void Update() override {
        // Handle input and update logic
    }

    void Render() override {
        // Render textured quad
    }

    void Cleanup() override {
        std::cout << "\nCleaning up texture render demo..." << std::endl;
        
        if (m_texture) {
            m_texture->Cleanup();
            m_texture.reset();
            std::cout << "✓ Texture cleaned up" << std::endl;
        }

        if (m_vertexShader) {
            m_vertexShader.reset();
            std::cout << "✓ Vertex shader cleaned up" << std::endl;
        }

        if (m_fragmentShader) {
            m_fragmentShader.reset();
            std::cout << "✓ Fragment shader cleaned up" << std::endl;
        }

        AquaEngine::Texture::CleanupStaticResources();
        std::cout << "✓ Static texture resources cleaned up" << std::endl;

        Aqua::Platform::Application::Cleanup();
        
        std::cout << "✓ Texture render demo cleanup complete" << std::endl;
    }
};

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    AquaEngine Texture Render Demo" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "This demo showcases texture loading and" << std::endl;
    std::cout << "rendering with Vulkan integration." << std::endl;
    std::cout << "========================================" << std::endl;

    TextureRenderDemoApp app;
    app.Run();

    std::cout << "\nTexture render demo finished." << std::endl;
    return 0;
}