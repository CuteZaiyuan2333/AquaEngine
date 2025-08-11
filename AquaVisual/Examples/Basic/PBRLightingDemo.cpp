#include <AquaVisual/AquaVisual.h>
#include <AquaVisual/Core/VulkanRenderer.h>
#include <AquaVisual/Resources/Mesh.h>
#include <AquaVisual/Primitives.h>
#include <AquaVisual/Math/Vector.h>
#include <AquaVisual/Math/Matrix.h>
#include <iostream>
#include <chrono>
#include <memory>
#include <vector>
#include <cmath>

class PBRLightingDemo {
private:
    AquaVisual::VulkanRenderer* m_renderer;
    std::unique_ptr<AquaVisual::Mesh> m_triangleMesh;
    float m_time;

public:
    PBRLightingDemo() : m_renderer(nullptr), m_time(0.0f) {}
    
    ~PBRLightingDemo() {
        Cleanup();
    }
    
    bool Initialize() {
        std::cout << "=== AquaVisual PBR Lighting Demo ===" << std::endl;
        
        if (!AquaVisual::Initialize()) {
            std::cerr << "Failed to initialize AquaVisual!" << std::endl;
            return false;
        }
        std::cout << "AquaVisual initialized" << std::endl;
        
        AquaVisual::RendererConfig config;
        config.width = 1200;
        config.height = 800;
        config.title = "AquaVisual - PBR Lighting Demo";
        config.enableValidation = true;
        config.enableVSync = true;
        
        m_renderer = new AquaVisual::VulkanRenderer();
        m_renderer->SetConfig(config);
        if (!m_renderer->Initialize()) {
            std::cerr << "Failed to initialize Vulkan renderer!" << std::endl;
            return false;
        }
        std::cout << "Vulkan renderer initialized" << std::endl;
        
        if (!CreateScene()) {
            std::cerr << "Failed to create scene!" << std::endl;
            return false;
        }
        std::cout << "Scene created" << std::endl;
        
        return true;
    }
    
    bool CreateScene() {
        try {
            // Create a simple triangle that should be visible
            m_triangleMesh = AquaVisual::Primitives::CreateTriangle(1.0f);
            
            if (!m_triangleMesh) {
                std::cerr << "Failed to create triangle mesh!" << std::endl;
                return false;
            }
            
            std::cout << "Created triangle with " << m_triangleMesh->GetVertexCount() 
                      << " vertices and " << m_triangleMesh->GetIndexCount() << " indices" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Exception in CreateScene: " << e.what() << std::endl;
            return false;
        }
    }
    
    void Update(float deltaTime) {
        m_time += deltaTime;
    }
    
    void Render() {
        if (!m_renderer->BeginFrame()) {
            return;
        }
        
        // Use a different clear color to distinguish from white
        m_renderer->Clear(0.2f, 0.3f, 0.4f, 1.0f); // Blue-gray background
        
        // Render the triangle
        if (m_triangleMesh) {
            m_renderer->RenderMesh(*m_triangleMesh);
        }
        
        m_renderer->EndFrame();
    }
    
    void Run() {
        std::cout << "\n=== Starting PBR Lighting Demo ===" << std::endl;
        std::cout << "Expected: Blue-gray background with a colored triangle" << std::endl;
        std::cout << "If you see pure white, there may be a shader or driver issue" << std::endl;
        std::cout << "\nControls:" << std::endl;
        std::cout << "• Close window to exit" << std::endl;
        
        int frameCount = 0;
        auto startTime = std::chrono::high_resolution_clock::now();
        auto lastFrameTime = startTime;
        
        while (!m_renderer->ShouldClose()) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
            lastFrameTime = currentTime;
            
            m_renderer->PollEvents();
            Update(deltaTime);
            Render();
            
            frameCount++;
            
            // Print status every 3 seconds
            if (frameCount % 180 == 0) {
                auto elapsed = std::chrono::duration<float>(currentTime - startTime).count();
                if (elapsed > 0) {
                    float fps = frameCount / elapsed;
                    std::cout << "Frame " << frameCount << ", FPS: " << fps 
                              << ", Time: " << elapsed << "s" << std::endl;
                    std::cout << "Background color: RGB(0.2, 0.3, 0.4) - should be blue-gray" << std::endl;
                }
            }
        }
        
        std::cout << "\nDemo completed! Total frames rendered: " << frameCount << std::endl;
    }
    
    void Cleanup() {
        m_triangleMesh.reset();
        
        if (m_renderer) {
            m_renderer->Shutdown();
            delete m_renderer;
            m_renderer = nullptr;
        }
        
        AquaVisual::Shutdown();
    }
};

int main() {
    PBRLightingDemo demo;
    
    if (!demo.Initialize()) {
        std::cerr << "Failed to initialize PBR lighting demo!" << std::endl;
        std::cout << "\nTroubleshooting tips:" << std::endl;
        std::cout << "1. Make sure Vulkan drivers are installed" << std::endl;
        std::cout << "2. Check if your GPU supports Vulkan" << std::endl;
        std::cout << "3. Try updating graphics drivers" << std::endl;
        return -1;
    }
    
    demo.Run();
    
    std::cout << "PBR Lighting Demo completed successfully!" << std::endl;
    std::cout << "If the window showed pure white instead of blue-gray background," << std::endl;
    std::cout << "there may be an issue with shaders or Vulkan drivers." << std::endl;
    
    return 0;
}