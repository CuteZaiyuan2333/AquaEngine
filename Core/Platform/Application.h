#pragma once

#include <string>
#include <memory>
#include "Window.h"

namespace Aqua {
namespace Renderer {
    class VulkanContext;
}

namespace Platform {

class Application {
public:
    Application();
    virtual ~Application();

    // Run the application
    void Run();

    // Virtual functions for subclasses to override
    virtual bool Initialize();
    virtual void Update();
    virtual void Render();
    virtual void Cleanup();

    // Get Vulkan context
    std::shared_ptr<Renderer::VulkanContext> GetVulkanContext() const;
    
    // Get window
    std::shared_ptr<AquaEngine::Platform::Window> GetWindow() const { return m_window; }

protected:
    std::shared_ptr<AquaEngine::Platform::Window> m_window;
    bool m_isRunning;
    
    std::shared_ptr<Renderer::VulkanContext> m_vulkanContext;
};

} // namespace Platform
} // namespace Aqua