#include "Window.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace AquaEngine {
    namespace Platform {
        
        static bool s_GLFWInitialized = false;
        
        static void GLFWErrorCallback(int error, const char* description) {
            std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
        }
        
        Window::Window(const WindowProperties& props) {
            Init(props);
        }
        
        Window::~Window() {
            Shutdown();
        }
        
        void Window::Init(const WindowProperties& props) {
            m_data.title = props.title;
            m_data.width = props.width;
            m_data.height = props.height;
            
            std::cout << "Creating window " << props.title << " (" << props.width << ", " << props.height << ")" << std::endl;
            
            if (!s_GLFWInitialized) {
                int success = glfwInit();
                if (!success) {
                    std::cerr << "Could not initialize GLFW!" << std::endl;
                    return;
                }
                glfwSetErrorCallback(GLFWErrorCallback);
                s_GLFWInitialized = true;
            }
            
            // 不创建OpenGL上下文，因为我们使用Vulkan
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            
            m_window = glfwCreateWindow((int)props.width, (int)props.height, m_data.title.c_str(), nullptr, nullptr);
            
            if (!m_window) {
                std::cerr << "Failed to create GLFW window!" << std::endl;
                glfwTerminate();
                return;
            }
            
            glfwSetWindowUserPointer(m_window, &m_data);
            
            // 设置窗口关闭回调
            glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                if (data.eventCallback) {
                    data.eventCallback();
                }
            });
        }
        
        void Window::Shutdown() {
            if (m_window) {
                glfwDestroyWindow(m_window);
                m_window = nullptr;
            }
        }
        
        void Window::OnUpdate() {
            glfwPollEvents();
        }
        
        bool Window::ShouldClose() const {
            return glfwWindowShouldClose(m_window);
        }
        
    }
}