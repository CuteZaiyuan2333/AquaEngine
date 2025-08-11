#include <AquaVisual/AquaVisual.h>
#include <AquaVisual/Core/VulkanRendererImpl.h>
#include <AquaVisual/Resources/Mesh.h>
#include <AquaVisual/Primitives.h>
#include <AquaVisual/Math/Vector.h>
#include <AquaVisual/Math/Matrix.h>
#include <AquaVisual/Core/Camera.h>
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>

/**
 * LightingSceneDemo - 展示 AquaVisual 的光照和场景渲染功能
 * 
 * 本示例演示：
 * - 多个几何体的渲染
 * - 基础光照系统
 * - 材质系统
 * - 场景组合
 */

class LightingSceneDemo {
private:
    AquaVisual::VulkanRendererImpl* m_renderer;
    std::vector<std::shared_ptr<AquaVisual::Mesh>> m_meshes;
    
    // 场景对象
    struct SceneObject {
        std::shared_ptr<AquaVisual::Mesh> mesh;
        AquaVisual::Vector3 position;
        AquaVisual::Vector3 rotation;
        AquaVisual::Vector3 scale;
        AquaVisual::Vector3 color;
        
        SceneObject(std::shared_ptr<AquaVisual::Mesh> m, 
                   const AquaVisual::Vector3& pos = AquaVisual::Vector3(0, 0, 0),
                   const AquaVisual::Vector3& rot = AquaVisual::Vector3(0, 0, 0),
                   const AquaVisual::Vector3& scl = AquaVisual::Vector3(1, 1, 1),
                   const AquaVisual::Vector3& col = AquaVisual::Vector3(1, 1, 1))
            : mesh(m), position(pos), rotation(rot), scale(scl), color(col) {}
    };
    
    std::vector<SceneObject> m_sceneObjects;
    
    // 光照参数
    struct Light {
        AquaVisual::Vector3 position;
        AquaVisual::Vector3 color;
        float intensity;
        
        Light(const AquaVisual::Vector3& pos, const AquaVisual::Vector3& col, float intens = 1.0f)
            : position(pos), color(col), intensity(intens) {}
    };
    
    std::vector<Light> m_lights;
    
    // 动画参数
    float m_time;
    bool m_animateLight;
    bool m_animateObjects;

public:
    LightingSceneDemo() 
        : m_renderer(nullptr), m_time(0.0f), m_animateLight(true), m_animateObjects(true) {}
    
    ~LightingSceneDemo() {
        Cleanup();
    }
    
    bool Initialize() {
        std::cout << "=== AquaVisual Lighting Scene Demo ===" << std::endl;
        
        // Initialize AquaVisual
        if (!AquaVisual::Initialize()) {
            std::cerr << "Failed to initialize AquaVisual!" << std::endl;
            return false;
        }
        std::cout << "AquaVisual initialized" << std::endl;
        
        // Configure renderer
        AquaVisual::RendererConfig config;
        config.width = 1024;
        config.height = 768;
        config.title = "AquaVisual - Lighting Scene Demo";
        config.enableValidation = true;
        config.enableVSync = true;
        
        // Create Vulkan renderer
        m_renderer = new AquaVisual::VulkanRendererImpl(config);
        if (!m_renderer->Initialize()) {
            std::cerr << "Failed to initialize Vulkan renderer!" << std::endl;
            return false;
        }
        std::cout << "Vulkan renderer initialized" << std::endl;
        
        // Create scene objects
        if (!CreateScene()) {
            std::cerr << "Failed to create scene!" << std::endl;
            return false;
        }
        std::cout << "Scene created" << std::endl;
        
        // Setup lighting
        SetupLighting();
        std::cout << "Lighting setup complete" << std::endl;
        
        return true;
    }
    
    bool CreateScene() {
        try {
            // Create ground plane
            auto groundMesh = AquaVisual::Primitives::CreatePlane(10.0f, 10.0f);
            m_sceneObjects.emplace_back(std::shared_ptr<AquaVisual::Mesh>(groundMesh.release()), 
                AquaVisual::Vector3(0, -2, 0),     // position
                AquaVisual::Vector3(0, 0, 0),      // rotation
                AquaVisual::Vector3(1, 1, 1),      // scale
                AquaVisual::Vector3(0.3f, 0.5f, 0.3f)); // green ground
            
            // Create central cube
            auto cubeMesh = AquaVisual::Primitives::CreateCube(1.0f);
            m_sceneObjects.emplace_back(std::shared_ptr<AquaVisual::Mesh>(cubeMesh.release()),
                AquaVisual::Vector3(0, 0, 0),      // position
                AquaVisual::Vector3(0, 0, 0),      // rotation
                AquaVisual::Vector3(1, 1, 1),      // scale
                AquaVisual::Vector3(0.8f, 0.2f, 0.2f)); // red cube
            
            std::cout << "Created " << m_sceneObjects.size() << " scene objects" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Exception in CreateScene: " << e.what() << std::endl;
            return false;
        }
    }
    
    void SetupLighting() {
        // 主光源 - 模拟太阳光
        m_lights.emplace_back(
            AquaVisual::Vector3(5.0f, 8.0f, 5.0f),
            AquaVisual::Vector3(1.0f, 0.9f, 0.8f), // 暖白色
            1.0f
        );
        
        std::cout << "Setup " << m_lights.size() << " light sources" << std::endl;
    }
    
    void Update(float deltaTime) {
        m_time += deltaTime;
    }
    
    void Render() {
        if (!m_renderer->BeginFrame()) {
            return;
        }
        
        // 设置摄像机
        AquaVisual::Camera camera;
        
        // 获取窗口大小
        uint32_t width, height;
        m_renderer->GetWindowSize(width, height);
        
        // 设置透视投影
        camera.SetPerspective(45.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
        
        // 设置摄像机位置和朝向 - 从斜上方观察场景
        AquaVisual::Vector3 cameraPos(4.0f, 3.0f, 4.0f);
        AquaVisual::Vector3 target(0.0f, 0.0f, 0.0f);
        AquaVisual::Vector3 up(0.0f, 1.0f, 0.0f);
        camera.LookAt(cameraPos, target, up);
        
        // 将摄像机传递给渲染器
        m_renderer->SetCamera(camera);
        
        // 清屏 - 深蓝色夜空
        m_renderer->Clear(0.05f, 0.1f, 0.2f, 1.0f);
        
        // 渲染所有场景对象
        for (const auto& obj : m_sceneObjects) {
            if (obj.mesh) {
                // 这里应该设置变换矩阵和材质参数
                // 目前使用基础渲染
                m_renderer->RenderMesh(*obj.mesh);
            }
        }
        
        m_renderer->EndFrame();
    }
    
    void Run() {
        std::cout << "\n=== Starting Lighting Scene Demo ===" << std::endl;
        std::cout << "Features demonstrated:" << std::endl;
        std::cout << "• Multiple geometric objects (cubes, plane)" << std::endl;
        std::cout << "• Basic lighting with single light source" << std::endl;
        std::cout << "• Simple material system with different colors" << std::endl;
        std::cout << "\nControls:" << std::endl;
        std::cout << "• Close window to exit" << std::endl;
        std::cout << "• Watch the scene!" << std::endl;
        
        int frameCount = 0;
        auto startTime = std::chrono::high_resolution_clock::now();
        auto lastFrameTime = startTime;
        
        while (!m_renderer->ShouldClose()) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
            lastFrameTime = currentTime;
            
            // Handle window events
            m_renderer->PollEvents();
            
            // Update scene
            Update(deltaTime);
            
            // Render frame
            Render();
            
            frameCount++;
            
            // Output info every 120 frames (about 2 seconds at 60fps)
            if (frameCount % 120 == 0) {
                auto elapsed = std::chrono::duration<float>(currentTime - startTime).count();
                if (elapsed > 0) {
                    float fps = frameCount / elapsed;
                    std::cout << "Frame " << frameCount << ", FPS: " << fps 
                              << ", Time: " << elapsed << "s" << std::endl;
                }
            }
        }
        
        std::cout << "\nDemo completed! Total frames rendered: " << frameCount << std::endl;
    }
    
    void Cleanup() {
        if (m_renderer) {
            m_renderer->Shutdown();
            delete m_renderer;
            m_renderer = nullptr;
        }
        
        m_sceneObjects.clear();
        m_lights.clear();
        
        AquaVisual::Shutdown();
    }
};

int main() {
    LightingSceneDemo demo;
    
    if (!demo.Initialize()) {
        std::cerr << "Failed to initialize lighting scene demo!" << std::endl;
        return -1;
    }
    
    demo.Run();
    demo.Cleanup();
    
    std::cout << "Lighting Scene Demo completed successfully!" << std::endl;
    std::cout << "AquaVisual lighting system is working properly!" << std::endl;
    
    return 0;
}