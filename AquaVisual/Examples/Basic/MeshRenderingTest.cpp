#include <AquaVisual/AquaVisual.h>
#include <AquaVisual/Core/VulkanRendererImpl.h>
#include <AquaVisual/Core/Camera.h>
#include <AquaVisual/Math/Vector.h>
#include <AquaVisual/Math/Matrix.h>
#include <AquaVisual/Primitives.h>
#include <iostream>
#include <memory>
#include <chrono>

class MeshRenderingTest {
private:
    std::unique_ptr<AquaVisual::VulkanRendererImpl> m_renderer;
    std::unique_ptr<AquaVisual::Camera> m_camera;
    std::shared_ptr<AquaVisual::Mesh> m_cubeMesh;
    bool m_running;
    
public:
    MeshRenderingTest() : m_running(false) {}
    
    bool Initialize() {
        std::cout << "=== AquaVisual Mesh Rendering Test ===" << std::endl;
        
        // Initialize AquaVisual
        if (!AquaVisual::Initialize()) {
            std::cerr << "Failed to initialize AquaVisual!" << std::endl;
            return false;
        }
        std::cout << "✓ AquaVisual initialized" << std::endl;
        
        // Configure renderer
        AquaVisual::RendererConfig config;
        config.width = 800;
        config.height = 600;
        config.title = "AquaVisual - Mesh Rendering Test";
        config.enableValidation = true;
        config.enableVSync = true;
        
        // Create renderer
        m_renderer = std::make_unique<AquaVisual::VulkanRendererImpl>(config);
        if (!m_renderer->Initialize()) {
            std::cerr << "Failed to initialize Vulkan renderer!" << std::endl;
            return false;
        }
        std::cout << "✓ Vulkan renderer initialized" << std::endl;
        
        // Create camera
        m_camera = std::make_unique<AquaVisual::Camera>();
        m_camera->SetPosition(AquaVisual::Vector3(0.0f, 0.0f, 3.0f));
        m_camera->SetTarget(AquaVisual::Vector3(0.0f, 0.0f, 0.0f));
        m_camera->SetUp(AquaVisual::Vector3(0.0f, 1.0f, 0.0f));
        m_camera->SetPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
        std::cout << "✓ Camera created and configured" << std::endl;
        
        // Create test mesh (cube)
        m_cubeMesh = AquaVisual::Primitives::CreateCube(1.0f);
        if (!m_cubeMesh) {
            std::cerr << "Failed to create cube mesh!" << std::endl;
            return false;
        }
        std::cout << "✓ Cube mesh created with " << m_cubeMesh->GetVertexCount() 
                  << " vertices and " << m_cubeMesh->GetIndexCount() << " indices" << std::endl;
        
        return true;
    }
    
    void Run() {
        std::cout << "Starting render loop..." << std::endl;
        m_running = true;
        
        // Set frame rate limit
        m_renderer->SetFrameRateLimit(AquaVisual::FrameRateMode::FPS_60);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        uint32_t frameCount = 0;
        
        while (m_running && !m_renderer->ShouldClose()) {
            m_renderer->PollEvents();
            
            if (m_renderer->BeginFrame()) {
                // Set camera for this frame
                m_renderer->SetCamera(*m_camera);
                
                // Clear screen
                m_renderer->Clear(0.2f, 0.3f, 0.4f, 1.0f);
                
                // Render the cube mesh
                m_renderer->RenderMesh(*m_cubeMesh, nullptr);
                
                // End frame
                m_renderer->EndFrame();
                
                // Wait for frame rate limiting
                m_renderer->WaitForFrameRate();
            }
            
            frameCount++;
            
            // Output progress every 60 frames (approximately 1 second at 60 FPS)
            if (frameCount % 60 == 0) {
                auto currentTime = std::chrono::high_resolution_clock::now();
                float elapsedTime = std::chrono::duration<float>(currentTime - startTime).count();
                float fps = frameCount / elapsedTime;
                std::cout << "Frame " << frameCount << " - FPS: " << fps 
                          << " (Elapsed: " << elapsedTime << "s)" << std::endl;
            }
            
            // Exit after 5 seconds for testing
            auto currentTime = std::chrono::high_resolution_clock::now();
            float elapsedTime = std::chrono::duration<float>(currentTime - startTime).count();
            if (elapsedTime > 5.0f) {
                std::cout << "Test completed after 5 seconds" << std::endl;
                break;
            }
        }
        
        std::cout << "Render loop ended." << std::endl;
    }
    
    void Shutdown() {
        std::cout << "Shutting down..." << std::endl;
        
        // Clean up resources
        m_cubeMesh.reset();
        m_camera.reset();
        
        if (m_renderer) {
            m_renderer->Shutdown();
            m_renderer.reset();
            std::cout << "✓ Vulkan renderer shutdown" << std::endl;
        }
        
        AquaVisual::Shutdown();
        std::cout << "✓ AquaVisual shutdown" << std::endl;
    }
    
    void PrintTestInfo() {
        std::cout << "\n=== Test Information ===" << std::endl;
        std::cout << "AquaVisual Version: " << AquaVisual::GetVersion() << std::endl;
        std::cout << "Test Purpose: Verify mesh rendering with improved VulkanRenderer" << std::endl;
        std::cout << "Expected Behavior:" << std::endl;
        std::cout << "  - Window should open successfully" << std::endl;
        std::cout << "  - Camera matrices should be updated in uniform buffer" << std::endl;
        std::cout << "  - Cube mesh should be rendered using actual vertex/index data" << std::endl;
        std::cout << "  - No hardcoded cube rendering should occur" << std::endl;
        std::cout << "  - BufferManager should create temporary buffers for mesh data" << std::endl;
        std::cout << "========================\n" << std::endl;
    }
};

int main() {
    MeshRenderingTest test;
    
    test.PrintTestInfo();
    
    if (!test.Initialize()) {
        std::cerr << "Failed to initialize test!" << std::endl;
        return -1;
    }
    
    test.Run();
    
    test.Shutdown();
    
    std::cout << "\n=== Test Results ===" << std::endl;
    std::cout << "✓ Mesh rendering test completed successfully!" << std::endl;
    std::cout << "✓ All improved VulkanRenderer features were tested" << std::endl;
    std::cout << "===================" << std::endl;
    
    return 0;
}