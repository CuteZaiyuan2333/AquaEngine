#include "../../Core/Platform/Application.h"
#include "../../Core/Renderer/TriangleRenderer.h"
#include <iostream>
#include <memory>

class HelloTriangleApp : public Aqua::Platform::Application {
private:
    std::unique_ptr<Aqua::Renderer::TriangleRenderer> m_triangleRenderer;

public:
    HelloTriangleApp() {
        m_triangleRenderer = std::make_unique<Aqua::Renderer::TriangleRenderer>();
    }

    ~HelloTriangleApp() override {
        Cleanup();
    }

    bool Initialize() override {
        std::cout << "Initializing Hello Triangle application..." << std::endl;

        if (!Aqua::Platform::Application::Initialize()) {
            std::cerr << "Failed to initialize base application" << std::endl;
            return false;
        }

        if (!m_triangleRenderer->Initialize(GetVulkanContext())) {
            std::cerr << "Failed to initialize triangle renderer" << std::endl;
            return false;
        }

        std::cout << "Hello Triangle application initialized successfully" << std::endl;
        return true;
    }

    void Update() override {
        // Basic update logic
    }

    void Render() override {
        if (!m_triangleRenderer->BeginFrame()) {
            return;
        }

        m_triangleRenderer->RenderTriangle();

        if (!m_triangleRenderer->EndFrame()) {
            std::cerr << "Failed to end frame" << std::endl;
        }
    }

    void Cleanup() override {
        if (m_triangleRenderer) {
            m_triangleRenderer.reset();
        }
        Aqua::Platform::Application::Cleanup();
    }
};

int main() {
    try {
        HelloTriangleApp app;
        
        if (!app.Initialize()) {
            std::cerr << "Failed to initialize application" << std::endl;
            return -1;
        }

        app.Run();
        
        std::cout << "Application finished successfully" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
}