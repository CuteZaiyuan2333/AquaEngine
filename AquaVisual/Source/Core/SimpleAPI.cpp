#include "AquaVisual/SimpleAPI.h"
#include "AquaVisual/AquaVisual.h"
#include "AquaVisual/Core/VulkanRendererImpl.h"
#include "AquaVisual/Resources/Mesh.h"
#include "AquaVisual/Resources/Primitives.h"
#include <iostream>
#include <chrono>

namespace AquaVisual {
namespace Simple {

// ============================================================================
// SimpleObject Implementation
// ============================================================================

SimpleObject::SimpleObject(ObjectType type, const Transform& transform, const Material& material)
    : m_type(type), m_transform(transform), m_material(material), 
      m_animationEnabled(false), m_rotationSpeed(0, 0, 0) {
}

SimpleObject::~SimpleObject() = default;

void SimpleObject::SetPosition(const Vector3& position) {
    m_transform.position = position;
}

void SimpleObject::SetRotation(const Vector3& rotation) {
    m_transform.rotation = rotation;
}

void SimpleObject::SetScale(const Vector3& scale) {
    m_transform.scale = scale;
}

void SimpleObject::SetTransform(const Transform& transform) {
    m_transform = transform;
}

void SimpleObject::SetMaterial(const Material& material) {
    m_material = material;
}

void SimpleObject::SetColor(const Vector3& color) {
    m_material.albedo = color;
}

// ============================================================================
// SimpleLight Implementation
// ============================================================================

SimpleLight::SimpleLight(LightType type, const Vector3& position, const Vector3& color)
    : m_type(type), m_position(position), m_direction(0, -1, 0), 
      m_color(color), m_intensity(1.0f), m_spotAngle(45.0f) {
}

SimpleLight::~SimpleLight() = default;

void SimpleLight::SetPosition(const Vector3& position) {
    m_position = position;
}

void SimpleLight::SetDirection(const Vector3& direction) {
    m_direction = direction;
}

void SimpleLight::SetColor(const Vector3& color) {
    m_color = color;
}

void SimpleLight::SetIntensity(float intensity) {
    m_intensity = intensity;
}

void SimpleLight::SetSpotAngle(float angle) {
    m_spotAngle = angle;
}

// ============================================================================
// SimpleScene Implementation
// ============================================================================

SimpleScene::SimpleScene()
    : m_ambientColor(0.1f, 0.1f, 0.1f), m_ambientIntensity(0.1f),
      m_backgroundColor(0.1f, 0.2f, 0.3f),
      m_cameraPosition(0, 2, 5), m_cameraTarget(0, 0, 0), m_cameraFOV(45.0f) {
}

SimpleScene::~SimpleScene() = default;

std::shared_ptr<SimpleObject> SimpleScene::AddObject(ObjectType type, 
                                                     const Transform& transform,
                                                     const Material& material) {
    auto object = std::make_shared<SimpleObject>(type, transform, material);
    m_objects.push_back(object);
    return object;
}

void SimpleScene::RemoveObject(std::shared_ptr<SimpleObject> object) {
    auto it = std::find(m_objects.begin(), m_objects.end(), object);
    if (it != m_objects.end()) {
        m_objects.erase(it);
    }
}

void SimpleScene::ClearObjects() {
    m_objects.clear();
}

std::shared_ptr<SimpleLight> SimpleScene::AddLight(LightType type, 
                                                   const Vector3& position,
                                                   const Vector3& color) {
    auto light = std::make_shared<SimpleLight>(type, position, color);
    m_lights.push_back(light);
    return light;
}

void SimpleScene::RemoveLight(std::shared_ptr<SimpleLight> light) {
    auto it = std::find(m_lights.begin(), m_lights.end(), light);
    if (it != m_lights.end()) {
        m_lights.erase(it);
    }
}

void SimpleScene::ClearLights() {
    m_lights.clear();
}

void SimpleScene::SetAmbientLight(const Vector3& color, float intensity) {
    m_ambientColor = color;
    m_ambientIntensity = intensity;
}

void SimpleScene::SetBackgroundColor(const Vector3& color) {
    m_backgroundColor = color;
}

void SimpleScene::SetCameraPosition(const Vector3& position) {
    m_cameraPosition = position;
}

void SimpleScene::SetCameraTarget(const Vector3& target) {
    m_cameraTarget = target;
}

void SimpleScene::SetCameraFOV(float fov) {
    m_cameraFOV = fov;
}

void SimpleScene::Update(float deltaTime) {
    // 更新动画对象
    for (auto& object : m_objects) {
        if (object->m_animationEnabled) {
            object->m_transform.rotation.x += object->m_rotationSpeed.x * deltaTime;
            object->m_transform.rotation.y += object->m_rotationSpeed.y * deltaTime;
            object->m_transform.rotation.z += object->m_rotationSpeed.z * deltaTime;
        }
    }
}

// ============================================================================
// SimpleRenderer Implementation
// ============================================================================

class SimpleRenderer::Impl {
public:
    VulkanRendererImpl* renderer;
    std::vector<std::shared_ptr<Mesh>> meshCache;
    bool initialized;
    
    Impl() : renderer(nullptr), initialized(false) {}
    
    ~Impl() {
        if (renderer) {
            renderer->Shutdown();
            delete renderer;
        }
        AquaVisual::Shutdown();
    }
    
    std::shared_ptr<Mesh> GetOrCreateMesh(ObjectType type) {
        // 简单的网格缓存系统
        size_t index = static_cast<size_t>(type);
        if (index >= meshCache.size()) {
            meshCache.resize(index + 1);
        }
        
        if (!meshCache[index]) {
            meshCache[index] = std::make_shared<Mesh>();
            
            switch (type) {
                case ObjectType::Cube: {
                    auto vertices = Primitives::CreateCube(1.0f);
                    meshCache[index]->SetVertices(vertices);
                    break;
                }
                case ObjectType::Sphere: {
                    auto vertices = Primitives::CreateSphere(1.0f, 16, 16);
                    meshCache[index]->SetVertices(vertices);
                    break;
                }
                case ObjectType::Plane: {
                    auto vertices = Primitives::CreatePlane(1.0f, 1.0f);
                    meshCache[index]->SetVertices(vertices);
                    break;
                }
                case ObjectType::Triangle: {
                    auto vertices = Primitives::CreateTriangle();
                    meshCache[index]->SetVertices(vertices);
                    break;
                }
            }
        }
        
        return meshCache[index];
    }
};

SimpleRenderer::SimpleRenderer() : m_impl(std::make_unique<Impl>()), m_initialized(false) {
}

SimpleRenderer::~SimpleRenderer() = default;

bool SimpleRenderer::Initialize(const Config& config) {
    if (m_initialized) {
        return true;
    }
    
    std::cout << "Initializing AquaVisual Simple Renderer..." << std::endl;
    
    // Initialize AquaVisual
    if (!AquaVisual::Initialize()) {
        std::cerr << "Failed to initialize AquaVisual!" << std::endl;
        return false;
    }
    
    // Configure renderer
    RendererConfig rendererConfig;
    rendererConfig.width = config.width;
    rendererConfig.height = config.height;
    rendererConfig.title = config.title;
    rendererConfig.enableValidation = config.enableValidation;
    rendererConfig.enableVSync = config.vsync;
    
    // Create Vulkan renderer
    m_impl->renderer = new VulkanRendererImpl(rendererConfig);
    if (!m_impl->renderer->Initialize()) {
        std::cerr << "Failed to initialize Vulkan renderer!" << std::endl;
        return false;
    }
    
    m_impl->initialized = true;
    m_initialized = true;
    
    std::cout << "✓ AquaVisual Simple Renderer initialized successfully" << std::endl;
    return true;
}

void SimpleRenderer::Shutdown() {
    if (m_impl->renderer) {
        m_impl->renderer->Shutdown();
        delete m_impl->renderer;
        m_impl->renderer = nullptr;
    }
    m_impl->initialized = false;
    m_initialized = false;
    AquaVisual::Shutdown();
}

bool SimpleRenderer::BeginFrame() {
    if (!m_initialized || !m_impl->renderer) {
        return false;
    }
    return m_impl->renderer->BeginFrame();
}

void SimpleRenderer::RenderScene(const SimpleScene& scene) {
    if (!m_initialized || !m_impl->renderer) {
        return;
    }
    
    // 设置背景色
    const auto& bg = scene.m_backgroundColor;
    m_impl->renderer->Clear(bg.x, bg.y, bg.z, 1.0f);
    
    // 渲染所有对象
    for (const auto& object : scene.GetObjects()) {
        auto mesh = m_impl->GetOrCreateMesh(object->GetType());
        if (mesh) {
            // 这里应该设置变换矩阵和材质
            // 目前使用基础渲染
            m_impl->renderer->RenderMesh(*mesh);
        }
    }
}

void SimpleRenderer::EndFrame() {
    if (m_initialized && m_impl->renderer) {
        m_impl->renderer->EndFrame();
    }
}

bool SimpleRenderer::ShouldClose() const {
    if (!m_initialized || !m_impl->renderer) {
        return true;
    }
    return m_impl->renderer->ShouldClose();
}

void SimpleRenderer::PollEvents() {
    if (m_initialized && m_impl->renderer) {
        m_impl->renderer->PollEvents();
    }
}

bool SimpleRenderer::RenderSceneLoop(const SimpleScene& scene, int maxFrames) {
    if (!m_initialized) {
        std::cerr << "Renderer not initialized!" << std::endl;
        return false;
    }
    
    std::cout << "Starting render loop..." << std::endl;
    
    int frameCount = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    auto lastFrameTime = startTime;
    
    // 创建场景的可变副本用于更新
    SimpleScene mutableScene = scene;
    
    while (!ShouldClose() && (maxFrames == 0 || frameCount < maxFrames)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
        
        // 处理事件
        PollEvents();
        
        // 更新场景
        mutableScene.Update(deltaTime);
        
        // 渲染
        if (BeginFrame()) {
            RenderScene(mutableScene);
            EndFrame();
        }
        
        frameCount++;
        
        // 输出进度信息
        if (frameCount % 120 == 0) {
            auto elapsed = std::chrono::duration<float>(currentTime - startTime).count();
            float fps = frameCount / elapsed;
            std::cout << "Frame " << frameCount << ", FPS: " << fps << std::endl;
        }
    }
    
    std::cout << "Render loop completed. Total frames: " << frameCount << std::endl;
    return true;
}

// ============================================================================
// QuickStart Implementation
// ============================================================================

namespace QuickStart {

std::unique_ptr<SimpleScene> CreateDemoScene() {
    auto scene = std::make_unique<SimpleScene>();
    
    // 设置环境
    scene->SetBackgroundColor(Vector3(0.1f, 0.15f, 0.25f)); // 深蓝色背景
    scene->SetAmbientLight(Vector3(0.3f, 0.3f, 0.4f), 0.2f); // 柔和的环境光
    
    // 设置相机
    scene->SetCameraPosition(Vector3(5, 3, 5));
    scene->SetCameraTarget(Vector3(0, 0, 0));
    
    // 创建地面
    auto ground = scene->AddObject(ObjectType::Plane, 
        Transform(Vector3(0, -1, 0), Vector3(0, 0, 0), Vector3(10, 1, 10)),
        Material(Vector3(0.3f, 0.5f, 0.3f), 0.0f, 0.8f, 0.1f)); // 绿色地面
    
    // 创建中心立方体
    auto centerCube = scene->AddObject(ObjectType::Cube,
        Transform(Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(1, 1, 1)),
        Material(Vector3(0.8f, 0.2f, 0.2f), 0.1f, 0.3f, 0.8f)); // 红色立方体
    centerCube->SetAnimationEnabled(true);
    centerCube->SetRotationSpeed(Vector3(0, 45, 0)); // 绕Y轴旋转
    
    // 创建轨道球体
    for (int i = 0; i < 3; ++i) {
        float angle = i * 120.0f;
        float radius = 3.0f;
        Vector3 pos(
            cos(angle * 3.14159f / 180.0f) * radius,
            0.5f,
            sin(angle * 3.14159f / 180.0f) * radius
        );
        
        Vector3 colors[] = {
            Vector3(0.2f, 0.8f, 0.2f), // 绿色
            Vector3(0.2f, 0.2f, 0.8f), // 蓝色
            Vector3(0.8f, 0.8f, 0.2f)  // 黄色
        };
        
        auto sphere = scene->AddObject(ObjectType::Sphere,
            Transform(pos, Vector3(0, 0, 0), Vector3(0.6f, 0.6f, 0.6f)),
            Material(colors[i], 0.2f, 0.4f, 0.6f));
        sphere->SetAnimationEnabled(true);
        sphere->SetRotationSpeed(Vector3(90, 60, 30));
    }
    
    // 添加装饰立方体
    for (int i = 0; i < 4; ++i) {
        float angle = i * 90.0f;
        float radius = 5.0f;
        Vector3 pos(
            cos(angle * 3.14159f / 180.0f) * radius,
            -0.5f,
            sin(angle * 3.14159f / 180.0f) * radius
        );
        
        scene->AddObject(ObjectType::Cube,
            Transform(pos, Vector3(0, 0, 0), Vector3(0.4f, 0.4f, 0.4f)),
            Material(Vector3(0.6f, 0.4f, 0.8f), 0.3f, 0.5f, 0.4f)); // 紫色
    }
    
    // 添加光源
    // 主光源 - 太阳光
    auto sunLight = scene->AddLight(LightType::Directional, 
        Vector3(5, 8, 5), Vector3(1.0f, 0.9f, 0.8f));
    
    // 辅助光源 - 环境光
    auto fillLight = scene->AddLight(LightType::Point,
        Vector3(-3, 4, -3), Vector3(0.4f, 0.6f, 1.0f));
    fillLight->SetIntensity(0.5f);
    
    // 动态光源
    auto dynamicLight = scene->AddLight(LightType::Point,
        Vector3(0, 3, 0), Vector3(1.0f, 0.3f, 0.3f));
    dynamicLight->SetIntensity(0.8f);
    
    return scene;
}

bool ShowScene(const SimpleScene& scene, 
               const SimpleRenderer::Config& config,
               float duration) {
    SimpleRenderer renderer;
    
    if (!renderer.Initialize(config)) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return false;
    }
    
    std::cout << "Displaying scene..." << std::endl;
    if (duration > 0) {
        std::cout << "Duration: " << duration << " seconds" << std::endl;
    } else {
        std::cout << "Close window to exit" << std::endl;
    }
    
    int maxFrames = 0;
    if (duration > 0) {
        maxFrames = static_cast<int>(duration * 60); // 假设60fps
    }
    
    bool success = renderer.RenderSceneLoop(scene, maxFrames);
    
    renderer.Shutdown();
    return success;
}

bool ShowDemoScene() {
    std::cout << "=== AquaVisual Quick Demo Scene ===" << std::endl;
    
    auto scene = CreateDemoScene();
    if (!scene) {
        std::cerr << "Failed to create demo scene!" << std::endl;
        return false;
    }
    
    SimpleRenderer::Config config(1024, 768, "AquaVisual - Quick Demo Scene");
    config.vsync = true;
    
    return ShowScene(*scene, config);
}

} // namespace QuickStart

} // namespace Simple
} // namespace AquaVisual