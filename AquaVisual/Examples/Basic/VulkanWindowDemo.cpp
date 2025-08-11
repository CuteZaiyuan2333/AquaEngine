#include <AquaVisual/AquaVisual.h>
#include <AquaVisual/Core/VulkanRenderer.h>
#include <AquaVisual/Core/RenderPipeline.h>
#include <AquaVisual/Resources/Mesh.h>
#include <iostream>

int main() {
    std::cout << "=== Vulkan Window Demo with Real Rendering ===" << std::endl;
    
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
    config.title = "AquaVisual - Vulkan Real Demo";
    
    // 创建真正的 Vulkan 渲染器
    AquaVisual::VulkanRenderer* renderer = new AquaVisual::VulkanRenderer();
    renderer->SetConfig(config);
    if (!renderer->Initialize()) {
        std::cerr << "Failed to initialize Vulkan renderer!" << std::endl;
        delete renderer;
        AquaVisual::Shutdown();
        return -1;
    }
    std::cout << "✓ Vulkan renderer and window initialized" << std::endl;
    
    // 创建一个简单的mesh对象（虽然VulkanRenderer会忽略它，但需要传递引用）
    auto mesh = AquaVisual::Mesh::CreateTriangle(1.0f);
    std::cout << "✓ Mesh created successfully" << std::endl;
    
    std::cout << "\nStarting render loop..." << std::endl;
    std::cout << "You should see a rotating cube!" << std::endl;
    std::cout << "Close the window to exit." << std::endl;
    
    // 渲染循环
    int frameCount = 0;
    while (!renderer->ShouldClose()) {
        // 处理窗口事件
        renderer->PollEvents();
        
        // 渲染帧
        if (renderer->BeginFrame()) {
            // 设置清屏颜色（深蓝色背景）
            renderer->Clear(0.1f, 0.1f, 0.3f, 1.0f);
            
            // VulkanRenderer的RenderMesh会渲染硬编码的立方体
            // mesh参数会被忽略，但需要传递一个有效的引用
            renderer->RenderMesh(*mesh);
            
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