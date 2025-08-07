#pragma once

#include <string>
#include <functional>

struct GLFWwindow;

namespace AquaEngine {
    namespace Platform {
        
        struct WindowProperties {
            std::string title;
            unsigned int width;
            unsigned int height;
            
            WindowProperties(const std::string& title = "AquaEngine",
                           unsigned int width = 1280,
                           unsigned int height = 720)
                : title(title), width(width), height(height) {}
        };
        
        class Window {
        public:
            using EventCallbackFn = std::function<void()>;
            
            Window(const WindowProperties& props);
            virtual ~Window();
            
            void OnUpdate();
            
            unsigned int GetWidth() const { return m_data.width; }
            unsigned int GetHeight() const { return m_data.height; }
            
            void SetEventCallback(const EventCallbackFn& callback) { m_data.eventCallback = callback; }
            
            GLFWwindow* GetNativeWindow() const { return m_window; }
            
            bool ShouldClose() const;
            
        private:
            virtual void Init(const WindowProperties& props);
            virtual void Shutdown();
            
        private:
            GLFWwindow* m_window;
            
            struct WindowData {
                std::string title;
                unsigned int width, height;
                EventCallbackFn eventCallback;
            };
            
            WindowData m_data;
        };
        
    }
}