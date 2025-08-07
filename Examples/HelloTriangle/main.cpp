#include "../../Core/Platform/Application.h"
#include "../../Core/Renderer/Renderer.h"
#include "../../Core/Math/Vector3.h"
#include "../../Core/Math/Matrix4.h"
#include <iostream>
#include <memory>

class HelloTriangleApp : public Aqua::Platform::Application {
private:
    std::unique_ptr<Aqua::Renderer::Renderer> m_renderer;

public:
    bool Initialize() override {
        // Call base class initialization method
        if (!Aqua::Platform::Application::Initialize()) {
            std::cerr << "Base class initialization failed" << std::endl;
            return false;
        }
        
        std::cout << "HelloTriangle application initialization" << std::endl;
        
        // Create renderer
        m_renderer = std::make_unique<Aqua::Renderer::Renderer>();
        if (!m_renderer->Initialize(GetVulkanContext())) {
            std::cerr << "Renderer initialization failed" << std::endl;
            return false;
        }
        
        // Test math library
        Aqua::Math::Vector3 v1(1.0f, 2.0f, 3.0f);
        Aqua::Math::Vector3 v2(4.0f, 5.0f, 6.0f);
        Aqua::Math::Vector3 result = v1 + v2;
        
        std::cout << "Vector addition test: (" << result.x << ", " << result.y << ", " << result.z << ")" << std::endl;
        
        // Test matrix
        Aqua::Math::Matrix4 identity;
        identity.Identity();
        std::cout << "Identity matrix created successfully" << std::endl;
        
        return true;
    }
    
    void Update() override {
        // Update logic
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
        std::cout << "HelloTriangle application cleanup" << std::endl;
        
        // Cleanup renderer
        if (m_renderer) {
            m_renderer->Cleanup();
            m_renderer.reset();
        }
        
        // Call base class cleanup method
        Aqua::Platform::Application::Cleanup();
    }
};

int main() {
    HelloTriangleApp app;
    app.Run();
    return 0;
}