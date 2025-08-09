#include <AquaVisual/AquaVisual.h>
#include <AquaVisual/Core/VulkanRendererImpl.h>
#include <AquaVisual/Core/Renderer.h>
#include <AquaVisual/Math/Vector.h>
#include <AquaVisual/Primitives.h>
#include <iostream>
#include <memory>
#include <chrono>

class VulkanDemo {
private:
    std::unique_ptr<AquaVisual::VulkanRendererImpl> m_renderer;
    bool m_running;
    
public:
    VulkanDemo() : m_running(false) {}
    
    bool Initialize() {
        std::cout << "AquaVisual Vulkan Demo" << std::endl;
        
        if (!AquaVisual::Initialize()) {
            std::cerr << "Failed to initialize AquaVisual!" << std::endl;
            return false;
        }
        std::cout << "AquaVisual initialized" << std::endl;
        
        AquaVisual::RendererConfig config;
        config.width = 800;
        config.height = 600;
        config.title = "AquaVisual - Vulkan Demo";
        config.enableValidation = true;
        config.enableVSync = true;
        
        m_renderer = std::make_unique<AquaVisual::VulkanRendererImpl>(config);
        if (!m_renderer->Initialize()) {
            std::cerr << "Failed to initialize Vulkan renderer!" << std::endl;
            return false;
        }
        std::cout << "Vulkan renderer and window initialized" << std::endl;
        
        return true;
    }
    
    void Run() {
        std::cout << "Starting render loop..." << std::endl;
        
        m_running = true;
        auto startTime = std::chrono::high_resolution_clock::now();
        uint32_t frameCount = 0;
        
        while (m_running && !m_renderer->ShouldClose()) {
            m_renderer->PollEvents();
            
            if (m_renderer->BeginFrame()) {
                m_renderer->Clear(0.1f, 0.1f, 0.3f, 1.0f);
                m_renderer->EndFrame();
            }
            
            frameCount++;
            
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
            if (elapsed >= 1) {
                std::cout << "FPS: " << frameCount << std::endl;
                frameCount = 0;
                startTime = currentTime;
            }
        }
        
        std::cout << "Render loop ended." << std::endl;
    }
    
    void Shutdown() {
        std::cout << "Shutting down..." << std::endl;
        
        if (m_renderer) {
            m_renderer->Shutdown();
            m_renderer.reset();
            std::cout << "Vulkan renderer shutdown" << std::endl;
        }
        
        AquaVisual::Shutdown();
        std::cout << "AquaVisual shutdown" << std::endl;
    }
    
    void PrintSystemInfo() {
        std::cout << "System Information" << std::endl;
        std::cout << "AquaVisual Version: " << AquaVisual::GetVersion() << std::endl;
        
        AquaVisual::Vector3 v1(1.0f, 2.0f, 3.0f);
        AquaVisual::Vector3 v2(4.0f, 5.0f, 6.0f);
        auto sum = v1 + v2;
        std::cout << "Math Test - Vector addition: (" << sum.x << ", " << sum.y << ", " << sum.z << ")" << std::endl;
        
        auto cube = AquaVisual::Primitives::CreateCube(1.0f);
        if (cube) {
            std::cout << "Geometry Test - Cube vertices: " << cube->GetVertexCount() << std::endl;
        }
    }
};

int main() {
    VulkanDemo demo;
    
    demo.PrintSystemInfo();
    
    if (!demo.Initialize()) {
        std::cerr << "Failed to initialize demo!" << std::endl;
        return -1;
    }
    
    demo.Run();
    
    demo.Shutdown();
    
    std::cout << "Demo completed successfully!" << std::endl;
    return 0;
}