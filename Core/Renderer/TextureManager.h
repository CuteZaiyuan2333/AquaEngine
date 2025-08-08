#pragma once

#include "Texture.h"
#include <memory>
#include <unordered_map>
#include <string>

namespace AquaEngine {

class TextureManager {
public:
    TextureManager() = default;
    ~TextureManager() = default;

    // Disable copy constructor and assignment operator
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    // Initialize the texture manager with VulkanContext
    void Initialize(Aqua::Renderer::VulkanContext* context);
    
    // Cleanup all textures and resources
    void Cleanup();

    // Load a texture from file (returns existing texture if already loaded)
    std::shared_ptr<Texture> LoadTexture(const std::string& filepath);
    
    // Get a texture by filepath (returns nullptr if not found)
    std::shared_ptr<Texture> GetTexture(const std::string& filepath);
    
    // Remove a texture from cache
    void UnloadTexture(const std::string& filepath);
    
    // Get number of loaded textures
    size_t GetLoadedTextureCount() const { return m_textures.size(); }

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    Aqua::Renderer::VulkanContext* m_vulkanContext = nullptr;
};

} // namespace AquaEngine