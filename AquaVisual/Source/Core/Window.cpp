#include "AquaVisual/Core/Window.h"
#include <iostream>
#include <stdexcept>

#ifdef AQUA_HAS_GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#else
#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#endif
#endif

namespace AquaVisual {

Window::Window(uint32_t width, uint32_t height, const std::string& title)
    : m_window(nullptr), m_width(width), m_height(height), m_title(title) {
#ifndef AQUA_HAS_GLFW
#ifdef _WIN32
    m_hwnd = nullptr;
    m_shouldClose = false;
#endif
#endif
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
#ifdef _WIN32
    // Windows API implementation
    const char CLASS_NAME[] = "AquaVisualWindow";
    
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassA(&wc)) {
        DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS) {
            std::cerr << "Failed to register window class, error: " << error << std::endl;
            return false;
        }
    }

    m_hwnd = CreateWindowExA(
        0,                              // Optional window styles
        CLASS_NAME,                     // Window class
        m_title.c_str(),               // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        CW_USEDEFAULT, CW_USEDEFAULT,   // Position
        m_width, m_height,              // Size
        nullptr,                        // Parent window
        nullptr,                        // Menu
        GetModuleHandle(nullptr),       // Instance handle
        this                           // Additional application data
    );

    if (m_hwnd == nullptr) {
        std::cerr << "Failed to create window" << std::endl;
        return false;
    }

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);

    std::cout << "Windows API window initialized: " << m_width << "x" << m_height << " - " << m_title << std::endl;
    return true;
#else
    std::cout << "Window stub initialized: " << m_width << "x" << m_height << " - " << m_title << std::endl;
    std::cout << "Note: GLFW not available, using stub implementation" << std::endl;
    return true;
#endif
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
#ifdef _WIN32
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    std::cout << "Windows API window shutdown" << std::endl;
#else
    std::cout << "Window stub shutdown" << std::endl;
#endif
#endif
}

bool Window::ShouldClose() const {
#ifdef AQUA_HAS_GLFW
    return m_window ? glfwWindowShouldClose(m_window) : true;
#else
#ifdef _WIN32
    return m_shouldClose;
#else
    return false;
#endif
#endif
}

void Window::PollEvents() {
#ifdef AQUA_HAS_GLFW
    glfwPollEvents();
#else
#ifdef _WIN32
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#endif
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
#ifdef _WIN32
    // Windows API需要的Vulkan扩展
    std::vector<const char*> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };
    return extensions;
#else
    return {};
#endif
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
#ifdef _WIN32
    if (!m_hwnd || !instance || !surface) {
        std::cerr << "Invalid parameters for CreateVulkanSurface (Windows API)" << std::endl;
        return false;
    }
    
    VkInstance vkInstance = static_cast<VkInstance>(instance);
    VkSurfaceKHR vkSurface;
    
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = m_hwnd;
    createInfo.hinstance = GetModuleHandle(nullptr);
    
    VkResult result = vkCreateWin32SurfaceKHR(vkInstance, &createInfo, nullptr, &vkSurface);
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create Win32 surface, VkResult: " << result << std::endl;
        return false;
    }
    
    *surface = static_cast<void*>(vkSurface);
    std::cout << "Vulkan surface created successfully via Windows API" << std::endl;
    return true;
#else
    std::cerr << "No window system available, cannot create Vulkan surface" << std::endl;
    return false;
#endif
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

#ifndef AQUA_HAS_GLFW
#ifdef _WIN32
// Windows API window procedure
LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Window* window = nullptr;
    
    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
        window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window) {
        switch (uMsg) {
        case WM_DESTROY:
            window->m_shouldClose = true;
            PostQuitMessage(0);
            return 0;
        case WM_CLOSE:
            window->m_shouldClose = true;
            if (window->m_events.onClose) {
                window->m_events.onClose();
            }
            return 0;
        case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                window->m_width = width;
                window->m_height = height;
                if (window->m_events.onResize) {
                    window->m_events.onResize(width, height);
                }
            }
            return 0;
        case WM_KEYDOWN:
        case WM_KEYUP:
            if (window->m_events.onKey) {
                int action = (uMsg == WM_KEYDOWN) ? 1 : 0; // 1 for press, 0 for release
                window->m_events.onKey(static_cast<int>(wParam), 0, action, 0);
            }
            return 0;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            if (window->m_events.onMouseButton) {
                int button = 0;
                if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP) button = 0;
                else if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP) button = 1;
                else if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP) button = 2;
                
                int action = (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN) ? 1 : 0;
                window->m_events.onMouseButton(button, action, 0);
            }
            return 0;
        case WM_MOUSEMOVE:
            if (window->m_events.onMouseMove) {
                int xPos = GET_X_LPARAM(lParam);
                int yPos = GET_Y_LPARAM(lParam);
                window->m_events.onMouseMove(static_cast<double>(xPos), static_cast<double>(yPos));
            }
            return 0;
        case WM_MOUSEWHEEL:
            if (window->m_events.onScroll) {
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                window->m_events.onScroll(0.0, static_cast<double>(delta) / WHEEL_DELTA);
            }
            return 0;
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
                EndPaint(hwnd, &ps);
            }
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
#endif
#endif

bool Window::IsKeyPressed(int key) const {
#ifdef AQUA_HAS_GLFW
    if (!m_window) {
        return false;
    }
    return glfwGetKey(m_window, key) == GLFW_PRESS;
#else
#ifdef _WIN32
    // Windows API key state check
    // Convert GLFW key codes to Windows virtual key codes
    int vkCode = key;
    if (key >= 65 && key <= 90) { // A-Z
        vkCode = key;
    } else if (key >= 48 && key <= 57) { // 0-9
        vkCode = key;
    }
    // Add more key mappings as needed
    
    return (GetAsyncKeyState(vkCode) & 0x8000) != 0;
#else
    return false;
#endif
#endif
}

} // namespace AquaVisual