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

    // 运行应用程序
    void Run();

    // 虚函数，供子类重写
    virtual bool Initialize();
    virtual void Update();
    virtual void Render();
    virtual void Cleanup();

    // 获取Vulkan上下文
    std::shared_ptr<Renderer::VulkanContext> GetVulkanContext() const;
    
    // 获取窗口
    std::shared_ptr<AquaEngine::Platform::Window> GetWindow() const { return m_window; }

protected:
    std::shared_ptr<AquaEngine::Platform::Window> m_window;
    bool m_isRunning;
    
    std::shared_ptr<Renderer::VulkanContext> m_vulkanContext;
};

} // namespace Platform
} // namespace Aqua