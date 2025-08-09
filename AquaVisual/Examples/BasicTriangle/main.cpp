#include "../../Include/AquaVisual/AquaVisual.h"
#include <iostream>
#include <memory>

int main() {
    std::cout << "AquaVisual Basic Triangle Example" << std::endl;
    std::cout << "=================================" << std::endl;
    
    // 初始化 AquaVisual
    if (!AquaVisual::Initialize()) {
        std::cerr << "Failed to initialize AquaVisual" << std::endl;
        return -1;
    }
    
    std::cout << "AquaVisual Version: " << AquaVisual::GetVersion() << std::endl;
    std::cout << "Vulkan Available: " << (AquaVisual::IsVulkanAvailable() ? "Yes" : "No") << std::endl;
    
    // 创建渲染器
    auto renderer = AquaVisual::CreateDefaultRenderer();
    if (!renderer) {
        std::cerr << "Failed to create renderer" << std::endl;
        AquaVisual::Shutdown();
        return -1;
    }
    
    // 创建相机
    auto camera = AquaVisual::CreatePerspectiveCamera(
        AquaVisual::DegreesToRadians(45.0f), // FOV
        800.0f / 600.0f,                     // 宽高比
        0.1f,                                // 近裁剪面
        100.0f                               // 远裁剪面
    );
    
    // 设置相机位置
    camera->SetPosition(AquaVisual::Vector3(0.0f, 0.0f, 3.0f));
    camera->SetTarget(AquaVisual::Vector3(0.0f, 0.0f, 0.0f));
    
    // 创建三角形网格
    auto triangleMesh = AquaVisual::CreateTriangleMesh();
    
    // 创建材质
    auto material = AquaVisual::CreateUnlitMaterial(AquaVisual::Vector4(1.0f, 0.0f, 0.0f, 1.0f)); // 红色
    
    // 设置渲染器相机
    renderer->SetCamera(camera);
    
    std::cout << "Starting render loop..." << std::endl;
    
    // 主渲染循环
    while (!renderer->ShouldClose()) {
        // 处理事件
        renderer->PollEvents();
        
        // 开始帧
        renderer->BeginFrame();
        
        // 清除屏幕
        renderer->Clear(AquaVisual::Vector4(0.2f, 0.3f, 0.3f, 1.0f)); // 深灰色背景
        
        // 渲染三角形
        renderer->RenderMesh(triangleMesh, material, AquaVisual::Matrix4::Identity());
        
        // 结束帧
        renderer->EndFrame();
    }
    
    std::cout << "Render loop ended" << std::endl;
    
    // 清理
    renderer.reset();
    AquaVisual::Shutdown();
    
    std::cout << "Example completed successfully" << std::endl;
    return 0;
}