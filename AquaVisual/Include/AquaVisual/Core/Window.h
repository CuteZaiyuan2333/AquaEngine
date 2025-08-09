#pragma once

#include <string>
#include <functional>
#include <cstdint>
#include <vector>

#ifdef AQUA_HAS_GLFW
struct GLFWwindow;
#else
// Define a placeholder type when GLFW is not available
struct GLFWwindow {};
#endif

namespace AquaVisual {

/**
 * @brief Window event callback types
 */
struct WindowEvents {
    std::function<void(int, int)> onResize;
    std::function<void(int, int, int, int)> onKey;
    std::function<void(int, int, int)> onMouseButton;
    std::function<void(double, double)> onMouseMove;
    std::function<void(double, double)> onScroll;
    std::function<void()> onClose;
};

/**
 * @brief Window class - Encapsulates GLFW window management
 */
class Window {
public:
    /**
     * @brief Constructor
     * @param width Window width
     * @param height Window height
     * @param title Window title
     */
    Window(uint32_t width, uint32_t height, const std::string& title);

    /**
     * @brief Destructor
     */
    ~Window();

    /**
     * @brief Initialize window
     * @return Whether initialization succeeded
     */
    bool Initialize();

    /**
     * @brief Cleanup window
     */
    void Shutdown();

    /**
     * @brief Check if window should close
     * @return Whether window should close
     */
    bool ShouldClose() const;

    /**
     * @brief Poll events
     */
    void PollEvents();

    /**
     * @brief Swap buffers
     */
    void SwapBuffers();

    /**
     * @brief Get window size
     * @param width Output width
     * @param height Output height
     */
    void GetSize(uint32_t& width, uint32_t& height) const;

    /**
     * @brief Get framebuffer size
     * @param width Output width
     * @param height Output height
     */
    void GetFramebufferSize(uint32_t& width, uint32_t& height) const;

    /**
     * @brief Set window size
     * @param width Width
     * @param height Height
     */
    void SetSize(uint32_t width, uint32_t height);

    /**
     * @brief Set window title
     * @param title Title
     */
    void SetTitle(const std::string& title);

    /**
     * @brief Get GLFW window handle
     * @return GLFW window pointer
     */
    GLFWwindow* GetNativeWindow() const { return m_window; }

    /**
     * @brief Set event callbacks
     * @param events Event callback structure
     */
    void SetEventCallbacks(const WindowEvents& events);

    /**
     * @brief Get required Vulkan extensions
     * @return Extension name list
     */
    static std::vector<const char*> GetRequiredVulkanExtensions();

    /**
     * @brief Create Vulkan surface
     * @param instance Vulkan instance
     * @param surface Output surface
     * @return Whether creation succeeded
     */
    bool CreateVulkanSurface(void* instance, void** surface);

private:
    GLFWwindow* m_window;
    uint32_t m_width;
    uint32_t m_height;
    std::string m_title;
    WindowEvents m_events;

#ifdef AQUA_HAS_GLFW
    // GLFW callback functions
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void WindowCloseCallback(GLFWwindow* window);
#endif
};

} // namespace AquaVisual