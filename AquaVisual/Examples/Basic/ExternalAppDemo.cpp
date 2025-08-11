/**
 * @file ExternalAppDemo.cpp
 * @brief 外部应用程序示例 - 展示如何使用AquaVisual简化API
 * 
 * 这个示例程序展示了外部应用程序如何轻松地使用AquaVisual
 * 来创建和渲染具有光照效果的3D场景。
 */

#include <AquaVisual/SimpleAPI.h>
#include <iostream>
#include <memory>

using namespace AquaVisual::Simple;

/**
 * @brief 示例1：最简单的使用方式 - 一行代码显示演示场景
 */
void Example1_QuickDemo() {
    std::cout << "\n=== Example 1: Quick Demo Scene ===" << std::endl;
    std::cout << "This is the simplest way to use AquaVisual - just one function call!" << std::endl;
    
    // 一行代码显示演示场景！
    if (QuickStart::ShowDemoScene()) {
        std::cout << "✓ Quick demo completed successfully!" << std::endl;
    } else {
        std::cout << "✗ Quick demo failed!" << std::endl;
    }
}

/**
 * @brief 示例2：创建自定义场景
 */
void Example2_CustomScene() {
    std::cout << "\n=== Example 2: Custom Scene Creation ===" << std::endl;
    std::cout << "Creating a custom scene with specific objects and lighting..." << std::endl;
    
    // 创建场景
    SimpleScene scene;
    
    // 设置环境
    scene.SetBackgroundColor(Vector3(0.05f, 0.1f, 0.15f)); // 深色背景
    scene.SetAmbientLight(Vector3(0.2f, 0.3f, 0.4f), 0.15f);
    
    // 设置相机
    scene.SetCameraPosition(Vector3(4, 3, 6));
    scene.SetCameraTarget(Vector3(0, 0, 0));
    scene.SetCameraFOV(50.0f);
    
    // 创建地面
    auto ground = scene.AddObject(ObjectType::Plane,
        Transform(Vector3(0, -1.5f, 0), Vector3(0, 0, 0), Vector3(8, 1, 8)),
        Material(Vector3(0.4f, 0.3f, 0.2f), 0.0f, 0.9f, 0.1f)); // 棕色地面
    
    // 创建一个金属立方体
    auto metalCube = scene.AddObject(ObjectType::Cube,
        Transform(Vector3(-2, 0, 0), Vector3(0, 0, 0), Vector3(1, 1, 1)),
        Material(Vector3(0.7f, 0.7f, 0.8f), 0.9f, 0.1f, 1.0f)); // 高金属度
    metalCube->SetAnimationEnabled(true);
    metalCube->SetRotationSpeed(Vector3(0, 30, 0));
    
    // 创建一个粗糙的球体
    auto roughSphere = scene.AddObject(ObjectType::Sphere,
        Transform(Vector3(2, 0, 0), Vector3(0, 0, 0), Vector3(1, 1, 1)),
        Material(Vector3(0.8f, 0.3f, 0.2f), 0.1f, 0.9f, 0.2f)); // 高粗糙度
    roughSphere->SetAnimationEnabled(true);
    roughSphere->SetRotationSpeed(Vector3(45, 0, 45));
    
    // 创建一个光滑的三角形
    auto smoothTriangle = scene.AddObject(ObjectType::Triangle,
        Transform(Vector3(0, 1, -2), Vector3(0, 0, 0), Vector3(2, 2, 2)),
        Material(Vector3(0.2f, 0.8f, 0.3f), 0.0f, 0.1f, 0.9f)); // 高镜面反射
    smoothTriangle->SetAnimationEnabled(true);
    smoothTriangle->SetRotationSpeed(Vector3(0, 60, 0));
    
    // 添加多个光源
    // 主光源 - 白色方向光
    auto mainLight = scene.AddLight(LightType::Directional,
        Vector3(3, 5, 3), Vector3(1.0f, 1.0f, 1.0f));
    mainLight->SetIntensity(1.2f);
    
    // 辅助光源 - 蓝色点光源
    auto blueLight = scene.AddLight(LightType::Point,
        Vector3(-3, 2, 2), Vector3(0.3f, 0.5f, 1.0f));
    blueLight->SetIntensity(0.8f);
    
    // 暖色光源 - 橙色点光源
    auto warmLight = scene.AddLight(LightType::Point,
        Vector3(3, 2, -2), Vector3(1.0f, 0.6f, 0.2f));
    warmLight->SetIntensity(0.6f);
    
    // 显示场景
    SimpleRenderer::Config config(1200, 800, "AquaVisual - Custom Scene Demo");
    config.vsync = true;
    config.enableValidation = false;
    
    if (QuickStart::ShowScene(scene, config)) {
        std::cout << "✓ Custom scene demo completed successfully!" << std::endl;
    } else {
        std::cout << "✗ Custom scene demo failed!" << std::endl;
    }
}

/**
 * @brief 示例3：程序化场景生成
 */
void Example3_ProceduralScene() {
    std::cout << "\n=== Example 3: Procedural Scene Generation ===" << std::endl;
    std::cout << "Generating a procedural scene with multiple objects..." << std::endl;
    
    SimpleScene scene;
    
    // 设置环境
    scene.SetBackgroundColor(Vector3(0.02f, 0.02f, 0.05f)); // 几乎黑色的背景
    scene.SetAmbientLight(Vector3(0.1f, 0.1f, 0.2f), 0.1f);
    
    // 设置相机
    scene.SetCameraPosition(Vector3(0, 8, 12));
    scene.SetCameraTarget(Vector3(0, 0, 0));
    
    // 创建地面网格
    for (int x = -3; x <= 3; ++x) {
        for (int z = -3; z <= 3; ++z) {
            if ((x + z) % 2 == 0) { // 棋盘模式
                auto tile = scene.AddObject(ObjectType::Plane,
                    Transform(Vector3(x * 2.0f, -2, z * 2.0f), 
                             Vector3(0, 0, 0), 
                             Vector3(1.8f, 1, 1.8f)),
                    Material(Vector3(0.8f, 0.8f, 0.8f), 0.0f, 0.7f, 0.3f));
            }
        }
    }
    
    // 创建螺旋形排列的立方体
    const int numCubes = 12;
    for (int i = 0; i < numCubes; ++i) {
        float angle = (i * 30.0f) * (3.14159f / 180.0f); // 30度间隔
        float radius = 2.0f + i * 0.3f; // 螺旋半径递增
        float height = i * 0.5f; // 高度递增
        
        Vector3 position(
            cos(angle) * radius,
            height,
            sin(angle) * radius
        );
        
        // 颜色渐变
        float t = static_cast<float>(i) / numCubes;
        Vector3 color(
            0.5f + 0.5f * sin(t * 3.14159f),
            0.5f + 0.5f * sin(t * 3.14159f + 2.0f),
            0.5f + 0.5f * sin(t * 3.14159f + 4.0f)
        );
        
        auto cube = scene.AddObject(ObjectType::Cube,
            Transform(position, Vector3(0, 0, 0), Vector3(0.5f, 0.5f, 0.5f)),
            Material(color, t * 0.8f, (1.0f - t) * 0.8f + 0.1f, 0.7f));
        
        cube->SetAnimationEnabled(true);
        cube->SetRotationSpeed(Vector3(45 + i * 10, 30 + i * 5, 60 - i * 8));
    }
    
    // 创建中心球体
    auto centerSphere = scene.AddObject(ObjectType::Sphere,
        Transform(Vector3(0, 2, 0), Vector3(0, 0, 0), Vector3(1.5f, 1.5f, 1.5f)),
        Material(Vector3(1.0f, 0.8f, 0.2f), 0.8f, 0.1f, 1.0f)); // 金色球体
    centerSphere->SetAnimationEnabled(true);
    centerSphere->SetRotationSpeed(Vector3(0, 20, 0));
    
    // 动态光源系统
    const int numLights = 4;
    for (int i = 0; i < numLights; ++i) {
        float angle = (i * 90.0f) * (3.14159f / 180.0f);
        Vector3 position(
            cos(angle) * 6.0f,
            4.0f,
            sin(angle) * 6.0f
        );
        
        Vector3 colors[] = {
            Vector3(1.0f, 0.2f, 0.2f), // 红色
            Vector3(0.2f, 1.0f, 0.2f), // 绿色
            Vector3(0.2f, 0.2f, 1.0f), // 蓝色
            Vector3(1.0f, 1.0f, 0.2f)  // 黄色
        };
        
        auto light = scene.AddLight(LightType::Point, position, colors[i]);
        light->SetIntensity(0.8f);
    }
    
    // 显示场景
    SimpleRenderer::Config config(1400, 900, "AquaVisual - Procedural Scene Demo");
    config.vsync = true;
    
    if (QuickStart::ShowScene(scene, config)) {
        std::cout << "✓ Procedural scene demo completed successfully!" << std::endl;
    } else {
        std::cout << "✗ Procedural scene demo failed!" << std::endl;
    }
}

/**
 * @brief 示例4：手动渲染循环控制
 */
void Example4_ManualRenderLoop() {
    std::cout << "\n=== Example 4: Manual Render Loop Control ===" << std::endl;
    std::cout << "Demonstrating manual control over the render loop..." << std::endl;
    
    // 创建渲染器
    SimpleRenderer renderer;
    SimpleRenderer::Config config(800, 600, "AquaVisual - Manual Render Loop");
    
    if (!renderer.Initialize(config)) {
        std::cout << "✗ Failed to initialize renderer!" << std::endl;
        return;
    }
    
    // 创建简单场景
    SimpleScene scene;
    scene.SetBackgroundColor(Vector3(0.1f, 0.1f, 0.1f));
    
    // 添加一个旋转的立方体
    auto cube = scene.AddObject(ObjectType::Cube,
        Transform(Vector3(0, 0, -3), Vector3(0, 0, 0), Vector3(1, 1, 1)),
        Material(Vector3(0.8f, 0.4f, 0.2f)));
    
    // 添加光源
    scene.AddLight(LightType::Directional, Vector3(1, 1, 1), Vector3(1, 1, 1));
    
    std::cout << "Manual render loop started. Close window to exit." << std::endl;
    
    int frameCount = 0;
    float totalTime = 0.0f;
    
    // 手动渲染循环
    while (!renderer.ShouldClose()) {
        // 处理事件
        renderer.PollEvents();
        
        // 更新场景
        float deltaTime = 1.0f / 60.0f; // 假设60fps
        totalTime += deltaTime;
        scene.Update(deltaTime);
        
        // 手动旋转立方体
        cube->SetRotation(Vector3(totalTime * 30, totalTime * 45, totalTime * 60));
        
        // 渲染
        if (renderer.BeginFrame()) {
            renderer.RenderScene(scene);
            renderer.EndFrame();
        }
        
        frameCount++;
        
        // 每2秒输出一次信息
        if (frameCount % 120 == 0) {
            std::cout << "Frame " << frameCount << ", Time: " << totalTime << "s" << std::endl;
        }
    }
    
    renderer.Shutdown();
    std::cout << "✓ Manual render loop completed! Total frames: " << frameCount << std::endl;
}

int main() {
    std::cout << "=== AquaVisual External Application Demo ===" << std::endl;
    std::cout << "This demo shows how external applications can easily use AquaVisual" << std::endl;
    std::cout << "to create and render 3D scenes with lighting effects." << std::endl;
    
    try {
        // 运行所有示例
        Example1_QuickDemo();
        Example2_CustomScene();
        Example3_ProceduralScene();
        Example4_ManualRenderLoop();
        
        std::cout << "\n=== All Examples Completed Successfully! ===" << std::endl;
        std::cout << "AquaVisual Simple API is working perfectly!" << std::endl;
        std::cout << "External applications can now easily create lighting scenes." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}