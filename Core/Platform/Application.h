#pragma once

#include <string>
#include <memory>

// 前向声明
struct GLFWwindow;

namespace Aqua {
namespace Renderer {
    class VulkanContext;
}

namespace Platform {

class Application {
public:
    Application(const std::string& title = "AquaEngine", uint32_t width = 800, uint32_t height = 600);
    virtual ~Application();

    // 运行应用程序
    void Run();

    // 虚函数，供子类重写
    virtual bool Initialize() { return true; }
    virtual void Update(float deltaTime) {}
    virtual void Render() {}
    virtual void Cleanup() {}

    // 窗口相关
    GLFWwindow* GetWindow() const { return m_window; }
    uint32_t GetWidth() const { return m_width; }
    uint32_t GetHeight() const { return m_height; }
    bool ShouldClose() const;

    // 获取Vulkan上下文
    Renderer::VulkanContext* GetVulkanContext() const { return m_vulkanContext.get(); }

protected:
    std::string m_title;
    uint32_t m_width;
    uint32_t m_height;
    GLFWwindow* m_window;
    
    std::unique_ptr<Renderer::VulkanContext> m_vulkanContext;

private:
    bool InitializeWindow();
    bool InitializeVulkan();
    void MainLoop();
    void CleanupWindow();

    // 回调函数
    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void ErrorCallback(int error, const char* description);

    bool m_framebufferResized = false;
    double m_lastTime = 0.0;
};

} // namespace Platform
} // namespace Aqua