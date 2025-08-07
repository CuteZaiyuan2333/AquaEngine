#include "../../Core/Platform/Application.h"
#include "../../Core/Renderer/Renderer.h"
#include <iostream>
#include <memory>

class TextureTestApp : public Aqua::Platform::Application {
private:
    std::unique_ptr<Aqua::Renderer::Renderer> m_renderer;

public:
    TextureTestApp() : Application("Texture Test", 800, 600) {}
    
    bool Initialize() override {
        if (!Aqua::Platform::Application::Initialize()) {
            std::cerr << "Base class initialization failed" << std::endl;
            return false;
        }
        
        std::cout << "=== Texture Test with Triangle ===" << std::endl;
        std::cout << "Application initialized successfully!" << std::endl;
        std::cout << "Window size: " << GetWidth() << "x" << GetHeight() << std::endl;
        
        // Create renderer
        m_renderer = std::make_unique<Aqua::Renderer::Renderer>();
        if (!m_renderer->Initialize(GetVulkanContext())) {
            std::cerr << "Renderer initialization failed" << std::endl;
            return false;
        }
        
        std::cout << "Renderer initialized successfully!" << std::endl;
        std::cout << "You should now see a colorful triangle!" << std::endl;
        std::cout << "=== Test Complete ===" << std::endl;
        
        return true;
    }
    
    void Update(float deltaTime) override {
        static float totalTime = 0.0f;
        static float lastPrintTime = 0.0f;
        totalTime += deltaTime;
        
        if (totalTime - lastPrintTime >= 5.0f) {
            std::cout << "Application running... Total time: " << (int)totalTime << "s" << std::endl;
            lastPrintTime = totalTime;
        }
    }
    
    void Render() override {
        if (m_renderer) {
            if (m_renderer->BeginFrame()) {
                m_renderer->RenderTriangle();
                m_renderer->EndFrame();
            }
        }
    }
    
    void Cleanup() override {
        std::cout << "TextureTest application cleanup" << std::endl;
        
        if (m_renderer) {
            m_renderer->Cleanup();
            m_renderer.reset();
        }
        
        Aqua::Platform::Application::Cleanup();
    }
};

int main() {
    std::cout << "Starting AquaEngine Basic Test..." << std::endl;
    
    try {
        TextureTestApp app;
        app.Run();
    } catch (const std::exception& e) {
        std::cerr << "Application error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}