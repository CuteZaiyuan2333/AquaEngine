# 🎮 AquaVisual API 使用示例

## 🚀 快速开始示例

### 基础渲染示例

```cpp
#include <AquaVisual/AquaVisual.h>

int main() {
    // 1. 初始化渲染器
    auto config = AquaVisual::RendererConfig{};
    config.api = AquaVisual::GraphicsAPI::Vulkan;
    config.enableVirtualGeometry = true;
    config.enableRayTracing = false;  // 可选特性
    
    auto renderer = AquaVisual::CreateRenderer(config);
    
    // 2. 创建窗口和交换链
    auto window = renderer->CreateWindow(1920, 1080, "AquaVisual Demo");
    
    // 3. 加载资源
    auto mesh = renderer->LoadMesh("assets/models/dragon.fbx");
    auto material = renderer->CreatePBRMaterial();
    material->SetAlbedoTexture(renderer->LoadTexture("assets/textures/dragon_albedo.png"));
    material->SetNormalTexture(renderer->LoadTexture("assets/textures/dragon_normal.png"));
    material->SetMetallicRoughness(0.8f, 0.2f);
    
    // 4. 设置相机
    auto camera = renderer->CreateCamera();
    camera->SetPosition({0, 0, 5});
    camera->SetTarget({0, 0, 0});
    camera->SetFOV(45.0f);
    
    // 5. 渲染循环
    while (window->IsOpen()) {
        window->PollEvents();
        
        renderer->BeginFrame();
        {
            renderer->SetCamera(camera);
            renderer->DrawMesh(mesh, material, AquaVisual::Transform::Identity());
        }
        renderer->EndFrame();
        
        renderer->Present();
    }
    
    return 0;
}
```

## 🎨 高级渲染特性示例

### 1. 虚拟几何体渲染

```cpp
#include <AquaVisual/VirtualGeometry.h>

void SetupVirtualGeometry() {
    auto virtualGeometry = renderer->GetVirtualGeometry();
    
    // 启用Nanite风格的虚拟几何体
    virtualGeometry->EnableNaniteRendering(true);
    
    // 配置簇大小和LOD参数
    virtualGeometry->SetClusterSize(128);  // 每簇128个三角形
    virtualGeometry->SetLODBias(1.0f);     // LOD偏移
    
    // 加载高精度模型
    auto highPolyMesh = renderer->LoadMesh("assets/models/statue_8M_triangles.fbx");
    
    // 创建虚拟网格
    auto virtualMesh = virtualGeometry->CreateVirtualMesh(highPolyMesh);
    
    // 设置LOD级别
    std::vector<AquaVisual::LODLevel> lodLevels = {
        {0.0f, 1.0f},    // 距离0-1: 100%细节
        {1.0f, 0.5f},    // 距离1-2: 50%细节
        {2.0f, 0.25f},   // 距离2-4: 25%细节
        {4.0f, 0.1f}     // 距离4+: 10%细节
    };
    virtualGeometry->SetLODLevels(virtualMesh, lodLevels);
    
    // 渲染虚拟几何体
    renderer->DrawVirtualMesh(virtualMesh, material, transform);
}
```

### 2. 全局光照设置

```cpp
#include <AquaVisual/GlobalIllumination.h>

void SetupGlobalIllumination() {
    auto gi = renderer->GetGlobalIllumination();
    
    // 选择GI技术
    gi->SetTechnique(AquaVisual::GITechnique::RTGI);
    
    // 配置RTGI参数
    AquaVisual::RTGISettings rtgiSettings;
    rtgiSettings.maxBounces = 3;
    rtgiSettings.samplesPerPixel = 1;
    rtgiSettings.denoiserStrength = 0.8f;
    gi->ConfigureRTGI(rtgiSettings);
    
    // 添加光源
    auto sunLight = renderer->CreateDirectionalLight();
    sunLight->SetDirection({-0.5f, -1.0f, -0.3f});
    sunLight->SetColor({1.0f, 0.95f, 0.8f});
    sunLight->SetIntensity(3.0f);
    
    auto pointLight = renderer->CreatePointLight();
    pointLight->SetPosition({2.0f, 1.0f, 0.0f});
    pointLight->SetColor({1.0f, 0.3f, 0.1f});
    pointLight->SetRadius(5.0f);
    
    // 更新GI
    gi->UpdateGI(scene);
}
```

### 3. 材质系统高级用法

```cpp
#include <AquaVisual/Materials.h>

void CreateAdvancedMaterials() {
    // 创建自定义着色器材质
    auto customShader = renderer->LoadShader("shaders/custom_pbr.hlsl");
    auto customMaterial = renderer->CreateMaterial(customShader);
    
    // 设置材质参数
    customMaterial->SetParameter("u_BaseColor", AquaVisual::Color{0.8f, 0.2f, 0.1f, 1.0f});
    customMaterial->SetParameter("u_Metallic", 0.9f);
    customMaterial->SetParameter("u_Roughness", 0.1f);
    customMaterial->SetParameter("u_EmissiveStrength", 2.0f);
    
    // 设置纹理
    customMaterial->SetTexture("t_Albedo", albedoTexture);
    customMaterial->SetTexture("t_Normal", normalTexture);
    customMaterial->SetTexture("t_MetallicRoughness", metallicRoughnessTexture);
    customMaterial->SetTexture("t_Emissive", emissiveTexture);
    
    // 创建玻璃材质
    auto glassMaterial = renderer->CreateTransparentMaterial();
    glassMaterial->SetTransparency(0.1f);
    glassMaterial->SetRefractionIndex(1.52f);  // 玻璃折射率
    glassMaterial->SetRoughness(0.0f);
    
    // 创建次表面散射材质
    auto sssShader = renderer->LoadShader("shaders/subsurface_scattering.hlsl");
    auto sssMaterial = renderer->CreateMaterial(sssShader);
    sssMaterial->SetParameter("u_SubsurfaceColor", AquaVisual::Color{1.0f, 0.8f, 0.7f, 1.0f});
    sssMaterial->SetParameter("u_SubsurfaceRadius", 0.5f);
}
```

## 🔧 性能优化示例

### 1. 批处理渲染

```cpp
#include <AquaVisual/Batching.h>

void BatchedRendering() {
    auto batchRenderer = renderer->GetBatchRenderer();
    
    // 开始批处理
    batchRenderer->BeginBatch();
    
    // 添加多个相同材质的对象
    for (int i = 0; i < 1000; ++i) {
        AquaVisual::Transform transform;
        transform.position = {i * 2.0f, 0, 0};
        batchRenderer->AddInstance(mesh, material, transform);
    }
    
    // 执行批处理渲染
    batchRenderer->ExecuteBatch();
    batchRenderer->EndBatch();
}
```

### 2. GPU驱动渲染

```cpp
#include <AquaVisual/GPUDriven.h>

void GPUDrivenRendering() {
    auto gpuCulling = renderer->GetGPUCulling();
    
    // 设置剔除参数
    gpuCulling->SetFrustumCulling(true);
    gpuCulling->SetOcclusionCulling(true);
    gpuCulling->SetDistanceCulling(100.0f);  // 100米外剔除
    
    // 准备实例数据
    std::vector<AquaVisual::InstanceData> instances;
    for (int i = 0; i < 10000; ++i) {
        AquaVisual::InstanceData instance;
        instance.transform = GenerateRandomTransform();
        instance.materialID = GetRandomMaterialID();
        instances.push_back(instance);
    }
    
    // 上传到GPU
    auto instanceBuffer = renderer->CreateInstanceBuffer(instances);
    
    // GPU剔除和渲染
    gpuCulling->CullAndRender(instanceBuffer, camera);
}
```

## 🎯 多API支持示例

### 运行时API切换

```cpp
#include <AquaVisual/MultiAPI.h>

void APISwitch() {
    // 检查支持的API
    auto supportedAPIs = AquaVisual::GetSupportedAPIs();
    
    for (auto api : supportedAPIs) {
        std::cout << "Supported API: " << AquaVisual::ToString(api) << std::endl;
    }
    
    // 当前使用Vulkan
    if (renderer->GetCurrentAPI() == AquaVisual::GraphicsAPI::Vulkan) {
        std::cout << "Currently using Vulkan" << std::endl;
        
        // 切换到DirectX 12 (如果支持)
        if (renderer->IsAPISupported(AquaVisual::GraphicsAPI::DirectX12)) {
            std::cout << "Switching to DirectX 12..." << std::endl;
            
            if (renderer->SwitchAPI(AquaVisual::GraphicsAPI::DirectX12)) {
                std::cout << "Successfully switched to DirectX 12" << std::endl;
            } else {
                std::cout << "Failed to switch API" << std::endl;
            }
        }
    }
}
```

### API特定优化

```cpp
void APISpecificOptimizations() {
    switch (renderer->GetCurrentAPI()) {
        case AquaVisual::GraphicsAPI::Vulkan: {
            // Vulkan特定优化
            auto vulkanExt = renderer->GetVulkanExtensions();
            vulkanExt->EnableMeshShaders(true);
            vulkanExt->EnableRayTracingPipeline(true);
            vulkanExt->SetMemoryBudget(4 * 1024 * 1024 * 1024);  // 4GB
            break;
        }
        
        case AquaVisual::GraphicsAPI::DirectX12: {
            // DirectX 12特定优化
            auto d3d12Ext = renderer->GetD3D12Extensions();
            d3d12Ext->EnableVariableRateShading(true);
            d3d12Ext->EnableDirectStorageAPI(true);
            d3d12Ext->SetGPUUploadHeapSize(256 * 1024 * 1024);  // 256MB
            break;
        }
        
        default:
            break;
    }
}
```

## 🔍 调试和性能分析示例

### 1. 渲染调试

```cpp
#include <AquaVisual/Debug.h>

void RenderDebugging() {
    auto debugRenderer = renderer->GetDebugRenderer();
    
    // 启用调试模式
    debugRenderer->SetDebugMode(AquaVisual::DebugMode::Wireframe);
    
    // 可视化边界框
    debugRenderer->DrawBoundingBoxes(true);
    
    // 可视化光源
    debugRenderer->DrawLightVolumes(true);
    
    // 显示渲染统计
    debugRenderer->ShowRenderStats(true);
    
    // 着色器热重载
    debugRenderer->EnableShaderHotReload(true);
    
    // 捕获帧
    if (Input::IsKeyPressed(Key::F12)) {
        debugRenderer->CaptureFrame("debug_frame.json");
    }
}
```

### 2. 性能分析

```cpp
#include <AquaVisual/Profiler.h>

void PerformanceProfiling() {
    auto profiler = renderer->GetProfiler();
    
    // 开始性能分析
    profiler->BeginFrame();
    
    {
        // GPU时间测量
        auto gpuTimer = profiler->BeginGPUEvent("Geometry Pass");
        renderer->RenderGeometry();
        // gpuTimer自动结束
    }
    
    {
        auto gpuTimer = profiler->BeginGPUEvent("Lighting Pass");
        renderer->RenderLighting();
    }
    
    {
        auto gpuTimer = profiler->BeginGPUEvent("Post Processing");
        renderer->RenderPostProcessing();
    }
    
    profiler->EndFrame();
    
    // 获取性能数据
    auto stats = profiler->GetFrameStats();
    std::cout << "Frame Time: " << stats.frameTime << "ms" << std::endl;
    std::cout << "GPU Time: " << stats.gpuTime << "ms" << std::endl;
    std::cout << "Draw Calls: " << stats.drawCalls << std::endl;
    std::cout << "Triangles: " << stats.triangles << std::endl;
    
    // 内存使用统计
    auto memStats = profiler->GetMemoryStats();
    std::cout << "GPU Memory Used: " << memStats.gpuMemoryUsed / (1024*1024) << "MB" << std::endl;
    std::cout << "CPU Memory Used: " << memStats.cpuMemoryUsed / (1024*1024) << "MB" << std::endl;
}
```

## 🎮 完整游戏示例

### 简单的3D场景

```cpp
#include <AquaVisual/AquaVisual.h>

class SimpleGame {
private:
    std::unique_ptr<AquaVisual::Renderer> m_renderer;
    std::unique_ptr<AquaVisual::Window> m_window;
    std::unique_ptr<AquaVisual::Camera> m_camera;
    std::unique_ptr<AquaVisual::Scene> m_scene;
    
public:
    bool Initialize() {
        // 创建渲染器
        AquaVisual::RendererConfig config;
        config.api = AquaVisual::GraphicsAPI::Vulkan;
        config.enableVirtualGeometry = true;
        config.enableGlobalIllumination = true;
        
        m_renderer = AquaVisual::CreateRenderer(config);
        m_window = m_renderer->CreateWindow(1920, 1080, "Simple Game");
        
        // 设置相机
        m_camera = m_renderer->CreateCamera();
        m_camera->SetPosition({0, 2, 5});
        m_camera->SetTarget({0, 0, 0});
        
        // 创建场景
        m_scene = m_renderer->CreateScene();
        
        // 加载资源
        LoadAssets();
        SetupLighting();
        
        return true;
    }
    
    void LoadAssets() {
        // 加载地面
        auto groundMesh = m_renderer->CreatePlaneMesh(20, 20);
        auto groundMaterial = m_renderer->CreatePBRMaterial();
        groundMaterial->SetAlbedoColor({0.3f, 0.3f, 0.3f, 1.0f});
        groundMaterial->SetRoughness(0.8f);
        
        auto groundEntity = m_scene->CreateEntity();
        groundEntity->AddComponent<AquaVisual::MeshComponent>(groundMesh);
        groundEntity->AddComponent<AquaVisual::MaterialComponent>(groundMaterial);
        
        // 加载模型
        auto dragonMesh = m_renderer->LoadMesh("assets/dragon.fbx");
        auto dragonMaterial = m_renderer->CreatePBRMaterial();
        dragonMaterial->SetAlbedoTexture(m_renderer->LoadTexture("assets/dragon_albedo.png"));
        dragonMaterial->SetNormalTexture(m_renderer->LoadTexture("assets/dragon_normal.png"));
        
        auto dragonEntity = m_scene->CreateEntity();
        dragonEntity->AddComponent<AquaVisual::MeshComponent>(dragonMesh);
        dragonEntity->AddComponent<AquaVisual::MaterialComponent>(dragonMaterial);
        dragonEntity->GetComponent<AquaVisual::TransformComponent>()->position = {0, 1, 0};
    }
    
    void SetupLighting() {
        // 太阳光
        auto sunLight = m_renderer->CreateDirectionalLight();
        sunLight->SetDirection({-0.3f, -1.0f, -0.2f});
        sunLight->SetColor({1.0f, 0.95f, 0.8f});
        sunLight->SetIntensity(3.0f);
        m_scene->AddLight(sunLight);
        
        // 环境光
        auto skybox = m_renderer->LoadHDRI("assets/sky.hdr");
        m_scene->SetSkybox(skybox);
        
        // 启用全局光照
        auto gi = m_renderer->GetGlobalIllumination();
        gi->SetTechnique(AquaVisual::GITechnique::SSGI);
    }
    
    void Update(float deltaTime) {
        // 相机控制
        HandleInput(deltaTime);
        
        // 更新场景
        m_scene->Update(deltaTime);
    }
    
    void Render() {
        m_renderer->BeginFrame();
        {
            m_renderer->SetCamera(m_camera.get());
            m_renderer->RenderScene(m_scene.get());
        }
        m_renderer->EndFrame();
        m_renderer->Present();
    }
    
    void Run() {
        while (m_window->IsOpen()) {
            m_window->PollEvents();
            
            static auto lastTime = std::chrono::high_resolution_clock::now();
            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
            
            Update(deltaTime);
            Render();
        }
    }
    
    void HandleInput(float deltaTime) {
        const float moveSpeed = 5.0f * deltaTime;
        const float rotateSpeed = 90.0f * deltaTime;
        
        auto input = m_window->GetInput();
        
        if (input->IsKeyPressed(AquaVisual::Key::W)) {
            m_camera->MoveForward(moveSpeed);
        }
        if (input->IsKeyPressed(AquaVisual::Key::S)) {
            m_camera->MoveBackward(moveSpeed);
        }
        if (input->IsKeyPressed(AquaVisual::Key::A)) {
            m_camera->MoveLeft(moveSpeed);
        }
        if (input->IsKeyPressed(AquaVisual::Key::D)) {
            m_camera->MoveRight(moveSpeed);
        }
        
        auto mouseDelta = input->GetMouseDelta();
        m_camera->Rotate(mouseDelta.x * rotateSpeed, mouseDelta.y * rotateSpeed);
    }
};

int main() {
    SimpleGame game;
    
    if (!game.Initialize()) {
        return -1;
    }
    
    game.Run();
    return 0;
}
```

这些示例展示了AquaVisual的设计理念：**既提供简单易用的高级API，又保留了完全的底层控制能力**。从简单的三角形渲染到复杂的虚拟几何体和全局光照，都能通过清晰的API实现。

你觉得这样的API设计如何？有什么地方需要调整或者想要深入了解的吗？