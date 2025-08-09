#include <AquaVisual/AquaVisual.h>
#include <AquaVisual/Core/VulkanRendererImpl.h>
#include <iostream>

int main() {
    std::cout << "=== Vulkan Window Demo ===" << std::endl;
    
    // 初始化 AquaVisual
    if (!AquaVisual::Initialize()) {
        std::cerr << "Failed to initialize AquaVisual!" << std::endl;
        return -1;
    }
    std::cout << "✓ AquaVisual initialized" << std::endl;
    
    // 配置渲染器
    AquaVisual::RendererConfig config;
    config.width = 800;
    config.height = 600;
    config.title = "AquaVisual - Vulkan Window Demo";
    
    // 创建 Vulkan 渲染器
    AquaVisual::VulkanRendererImpl* renderer = new AquaVisual::VulkanRendererImpl(config);
    if (!renderer->Initialize()) {
        std::cerr << "Failed to initialize Vulkan renderer!" << std::endl;
        delete renderer;
        AquaVisual::Shutdown();
        return -1;
    }
    std::cout << "✓ Vulkan renderer and window initialized" << std::endl;
    
    std::cout << "\nStarting render loop..." << std::endl;
    std::cout << "Close the window to exit." << std::endl;
    
    // 渲染循环
    int frameCount = 0;
    while (!renderer->ShouldClose()) {
        // 处理窗口事件
        renderer->PollEvents();
        
        // 渲染帧
        if (renderer->BeginFrame()) {
            // 设置清屏颜色（深蓝色）
            renderer->Clear(0.1f, 0.1f, 0.3f, 1.0f);
            renderer->EndFrame();
        }
        
        frameCount++;
        
        // 每100帧输出一次信息
        if (frameCount % 100 == 0) {
            std::cout << "Rendered " << frameCount << " frames" << std::endl;
        }
    }
    
    std::cout << "\nRender loop ended. Total frames: " << frameCount << std::endl;
    
    // 清理
    renderer->Shutdown();
    delete renderer;
    AquaVisual::Shutdown();
    
    std::cout << "✓ Demo completed successfully!" << std::endl;
    return 0;
}