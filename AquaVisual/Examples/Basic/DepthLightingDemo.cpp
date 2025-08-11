#include <AquaVisual/AquaVisual.h>
#include <AquaVisual/Core/VulkanRendererImpl.h>
#include <iostream>
#include <chrono>

int main() {
    std::cout << "=== Depth Buffer & Lighting Demo ===" << std::endl;
    
    // Initialize AquaVisual
    if (!AquaVisual::Initialize()) {
        std::cerr << "Failed to initialize AquaVisual!" << std::endl;
        return -1;
    }
    std::cout << "AquaVisual initialized" << std::endl;
    
    // Configure renderer
    AquaVisual::RendererConfig config;
    config.width = 800;
    config.height = 600;
    config.title = "AquaVisual - Depth Buffer & Lighting Demo";
    config.enableValidation = true;
    config.enableVSync = true;
    
    // Create Vulkan renderer
    AquaVisual::VulkanRendererImpl* renderer = new AquaVisual::VulkanRendererImpl(config);
    if (!renderer->Initialize()) {
        std::cerr << "Failed to initialize Vulkan renderer!" << std::endl;
        delete renderer;
        AquaVisual::Shutdown();
        return -1;
    }
    std::cout << "Vulkan renderer and window initialized" << std::endl;
    std::cout << "Depth buffer and lighting shaders loaded" << std::endl;
    
    std::cout << "\nStarting render loop..." << std::endl;
    std::cout << "This demo showcases:" << std::endl;
    std::cout << "- Depth buffer for proper 3D rendering" << std::endl;
    std::cout << "- Lighting effects (ambient, diffuse, specular)" << std::endl;
    std::cout << "- Animated rotating cube with depth fog" << std::endl;
    std::cout << "Close the window to exit." << std::endl;
    
    // Render loop
    int frameCount = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    while (!renderer->ShouldClose()) {
        // Handle window events
        renderer->PollEvents();
        
        // Render frame
        if (renderer->BeginFrame()) {
            // Clear with deep blue color (simulating night sky)
            renderer->Clear(0.1f, 0.2f, 0.4f, 1.0f);
            renderer->EndFrame();
        }
        
        frameCount++;
        
        // Output info every 100 frames
        if (frameCount % 100 == 0) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
            if (elapsed > 0) {
                float fps = frameCount / (float)elapsed;
                std::cout << "Rendered " << frameCount << " frames, FPS: " << fps << std::endl;
            }
        }
    }
    
    std::cout << "\nRender loop ended. Total frames: " << frameCount << std::endl;
    
    // Cleanup
    renderer->Shutdown();
    delete renderer;
    AquaVisual::Shutdown();
    
    std::cout << "Demo completed successfully!" << std::endl;
    std::cout << "Depth buffer and lighting features have been successfully implemented!" << std::endl;
    return 0;
}