#include "AquaVisual/Core/Renderer.h"
#include "AquaVisual/Core/VulkanRenderer.h"
#include <memory>

namespace AquaVisual {

std::unique_ptr<Renderer> Renderer::Create(const RendererConfig& config) {
    // Create Vulkan renderer implementation
    auto renderer = std::make_unique<VulkanRenderer>();
    
    // Set configuration
    renderer->SetConfig(config);
    
    // Initialize renderer
    if (!renderer->Initialize()) {
        return nullptr;
    }
    
    return std::move(renderer);
}

}