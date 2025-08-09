#include "AquaVisual/Core/Window.h"
#include <iostream>
#include <stdexcept>

#ifdef AQUA_HAS_GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

namespace AquaVisual {

Window::Window(uint32_t width, uint32_t height, const std::string& title)
    : m_window(nullptr), m_width(width), m_height(height), m_title(title) {
}

Window::~Window() {
    Shutdown();
}

bool Window::Initialize() {
#ifdef AQUA_HAS_GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // 对于 Vulkan
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
    glfwSetKeyCallback(m_window, KeyCallback);
    glfwSetMouseButtonCallback(m_window, MouseButtonCallback);
    glfwSetCursorPosCallback(m_window, CursorPosCallback);
    glfwSetScrollCallback(m_window, ScrollCallback);
    glfwSetWindowCloseCallback(m_window, WindowCloseCallback);

    std::cout << "Window initialized: " << m_width << "x" << m_height << " - " << m_title << std::endl;
    return true;
#else
    std::cout << "Window stub initialized: " << m_width << "x" << m_height << " - " << m_title << std::endl;
    std::cout << "Note: GLFW not available, using stub implementation" << std::endl;
    return true;
#endif
}

void Window::Shutdown() {
#ifdef AQUA_HAS_GLFW
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
#else
    std::cout << "Window stub shutdown" << std::endl;
#endif
}

bool Window::ShouldClose() const {
#ifdef AQUA_HAS_GLFW
    return m_window ? glfwWindowShouldClose(m_window) : true;
#else
    return false;
#endif
}

void Window::PollEvents() {
#ifdef AQUA_HAS_GLFW
    glfwPollEvents();
#endif
}

void Window::SwapBuffers() {
    // Vulkan doesn't need buffer swapping, but keep interface for future use
}

void Window::GetSize(uint32_t& width, uint32_t& height) const {
#ifdef AQUA_HAS_GLFW
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    width = static_cast<uint32_t>(w);
    height = static_cast<uint32_t>(h);
#else
    width = m_width;
    height = m_height;
#endif
}

void Window::GetFramebufferSize(uint32_t& width, uint32_t& height) const {
#ifdef AQUA_HAS_GLFW
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    width = static_cast<uint32_t>(w);
    height = static_cast<uint32_t>(h);
#else
    width = m_width;
    height = m_height;
#endif
}

void Window::SetSize(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
#ifdef AQUA_HAS_GLFW
    if (m_window) {
        glfwSetWindowSize(m_window, width, height);
    }
#endif
}

void Window::SetTitle(const std::string& title) {
    m_title = title;
#ifdef AQUA_HAS_GLFW
    if (m_window) {
        glfwSetWindowTitle(m_window, title.c_str());
    }
#endif
}

void Window::SetEventCallbacks(const WindowEvents& events) {
    m_events = events;
}

std::vector<const char*> Window::GetRequiredVulkanExtensions() {
#ifdef AQUA_HAS_GLFW
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    return extensions;
#else
    return {};
#endif
}

bool Window::CreateVulkanSurface(void* instance, void** surface) {
#ifdef AQUA_HAS_GLFW
    if (!m_window || !instance || !surface) {
        std::cerr << "Invalid parameters for CreateVulkanSurface" << std::endl;
        return false;
    }
    
    VkInstance vkInstance = static_cast<VkInstance>(instance);
    VkSurfaceKHR vkSurface;
    
    VkResult result = glfwCreateWindowSurface(vkInstance, m_window, nullptr, &vkSurface);
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create window surface, VkResult: " << result << std::endl;
        return false;
    }
    
    *surface = static_cast<void*>(vkSurface);
    std::cout << "Vulkan surface created successfully via GLFW" << std::endl;
    return true;
#else
    std::cerr << "GLFW not available, cannot create Vulkan surface" << std::endl;
    return false;
#endif
}

#ifdef AQUA_HAS_GLFW
void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win && win->m_events.onResize) {
        win->m_events.onResize(width, height);
    }
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win && win->m_events.onKey) {
        win->m_events.onKey(key, scancode, action, mods);
    }
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win && win->m_events.onMouseButton) {
        win->m_events.onMouseButton(button, action, mods);
    }
}

void Window::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win && win->m_events.onMouseMove) {
        win->m_events.onMouseMove(xpos, ypos);
    }
}

void Window::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win && win->m_events.onScroll) {
        win->m_events.onScroll(xoffset, yoffset);
    }
}

void Window::WindowCloseCallback(GLFWwindow* window) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win && win->m_events.onClose) {
        win->m_events.onClose();
    }
}
#endif

} // namespace AquaVisual