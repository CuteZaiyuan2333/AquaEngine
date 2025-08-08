#include "TextureManager.h"
#include <iostream>

namespace AquaEngine {

void TextureManager::Initialize(Aqua::Renderer::VulkanContext* context) {
    if (!context) {
        std::cerr << "Error: Null VulkanContext provided to TextureManager" << std::endl;
        return;
    }
    
    m_vulkanContext = context;
    Texture::SetVulkanContext(context);
    
    std::cout << "TextureManager initialized successfully" << std::endl;
}

void TextureManager::Cleanup() {
    std::cout << "Cleaning up TextureManager with " << m_textures.size() << " textures" << std::endl;
    
    // Clear all textures (shared_ptr will handle cleanup automatically)
    m_textures.clear();
    
    // Cleanup static texture resources
    Texture::CleanupStaticResources();
    
    m_vulkanContext = nullptr;
    
    std::cout << "TextureManager cleanup complete" << std::endl;
}

std::shared_ptr<Texture> TextureManager::LoadTexture(const std::string& filepath) {
    if (filepath.empty()) {
        std::cerr << "Error: Empty filepath provided to LoadTexture" << std::endl;
        return nullptr;
    }
    
    if (!m_vulkanContext) {
        std::cerr << "Error: TextureManager not initialized" << std::endl;
        return nullptr;
    }
    
    // Check if texture is already loaded
    auto it = m_textures.find(filepath);
    if (it != m_textures.end()) {
        std::cout << "Texture already loaded, returning cached version: " << filepath << std::endl;
        return it->second;
    }
    
    // Create new texture
    auto texture = std::make_shared<Texture>();
    if (!texture->LoadFromFile(filepath)) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        return nullptr;
    }
    
    // Cache the texture
    m_textures[filepath] = texture;
    std::cout << "Texture loaded and cached: " << filepath << std::endl;
    
    return texture;
}

std::shared_ptr<Texture> TextureManager::GetTexture(const std::string& filepath) {
    auto it = m_textures.find(filepath);
    if (it != m_textures.end()) {
        return it->second;
    }
    return nullptr;
}

void TextureManager::UnloadTexture(const std::string& filepath) {
    auto it = m_textures.find(filepath);
    if (it != m_textures.end()) {
        std::cout << "Unloading texture: " << filepath << std::endl;
        m_textures.erase(it);
    }
}

} // namespace AquaEngine