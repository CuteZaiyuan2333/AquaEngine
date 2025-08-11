#pragma once

/**
 * @file SimpleAPI.h
 * @brief 简化的AquaVisual API，专为外部应用程序设计
 * 
 * 这个API提供了一个简单易用的接口，让外部应用程序可以快速
 * 创建和渲染具有光照效果的3D场景。
 */

#include "AquaVisual/Math/Vector.h"
#include <memory>
#include <vector>
#include <string>

namespace AquaVisual {
namespace Simple {

// 前向声明
class SimpleRenderer;
class SimpleScene;
class SimpleObject;
class SimpleLight;

/**
 * @brief 简单的3D对象类型
 */
enum class ObjectType {
    Cube,
    Sphere,
    Plane,
    Triangle
};

/**
 * @brief 光源类型
 */
enum class LightType {
    Directional,  // 方向光（如太阳光）
    Point,        // 点光源
    Spot          // 聚光灯
};

/**
 * @brief 材质属性
 */
struct Material {
    Vector3 albedo;          // 基础颜色
    float metallic;          // 金属度 (0.0 - 1.0)
    float roughness;         // 粗糙度 (0.0 - 1.0)
    float specular;          // 镜面反射强度
    
    Material(const Vector3& color = Vector3(1, 1, 1), 
             float metal = 0.0f, 
             float rough = 0.5f, 
             float spec = 0.5f)
        : albedo(color), metallic(metal), roughness(rough), specular(spec) {}
};

/**
 * @brief 变换信息
 */
struct Transform {
    Vector3 position;
    Vector3 rotation;  // 欧拉角 (度)
    Vector3 scale;
    
    Transform(const Vector3& pos = Vector3(0, 0, 0),
              const Vector3& rot = Vector3(0, 0, 0),
              const Vector3& scl = Vector3(1, 1, 1))
        : position(pos), rotation(rot), scale(scl) {}
};

/**
 * @brief 简单的3D对象
 */
class SimpleObject {
public:
    SimpleObject(ObjectType type, const Transform& transform = Transform(), 
                 const Material& material = Material());
    ~SimpleObject();
    
    // 设置变换
    void SetPosition(const Vector3& position);
    void SetRotation(const Vector3& rotation);
    void SetScale(const Vector3& scale);
    void SetTransform(const Transform& transform);
    
    // 设置材质
    void SetMaterial(const Material& material);
    void SetColor(const Vector3& color);
    
    // 获取属性
    const Transform& GetTransform() const { return m_transform; }
    const Material& GetMaterial() const { return m_material; }
    ObjectType GetType() const { return m_type; }
    
    // 动画支持
    void SetAnimationEnabled(bool enabled) { m_animationEnabled = enabled; }
    void SetRotationSpeed(const Vector3& speed) { m_rotationSpeed = speed; }
    
private:
    ObjectType m_type;
    Transform m_transform;
    Material m_material;
    bool m_animationEnabled;
    Vector3 m_rotationSpeed;
    
    friend class SimpleScene;
};

/**
 * @brief 简单的光源
 */
class SimpleLight {
public:
    SimpleLight(LightType type, const Vector3& position, const Vector3& color = Vector3(1, 1, 1));
    ~SimpleLight();
    
    // 设置属性
    void SetPosition(const Vector3& position);
    void SetDirection(const Vector3& direction); // 仅对方向光和聚光灯有效
    void SetColor(const Vector3& color);
    void SetIntensity(float intensity);
    
    // 聚光灯特有属性
    void SetSpotAngle(float angle);  // 聚光灯锥角（度）
    
    // 获取属性
    LightType GetType() const { return m_type; }
    const Vector3& GetPosition() const { return m_position; }
    const Vector3& GetColor() const { return m_color; }
    float GetIntensity() const { return m_intensity; }
    
private:
    LightType m_type;
    Vector3 m_position;
    Vector3 m_direction;
    Vector3 m_color;
    float m_intensity;
    float m_spotAngle;
    
    friend class SimpleScene;
};

/**
 * @brief 简单的场景管理器
 */
class SimpleScene {
public:
    SimpleScene();
    ~SimpleScene();
    
    // 对象管理
    std::shared_ptr<SimpleObject> AddObject(ObjectType type, 
                                           const Transform& transform = Transform(),
                                           const Material& material = Material());
    void RemoveObject(std::shared_ptr<SimpleObject> object);
    void ClearObjects();
    
    // 光源管理
    std::shared_ptr<SimpleLight> AddLight(LightType type, 
                                         const Vector3& position,
                                         const Vector3& color = Vector3(1, 1, 1));
    void RemoveLight(std::shared_ptr<SimpleLight> light);
    void ClearLights();
    
    // 场景设置
    void SetAmbientLight(const Vector3& color, float intensity = 0.1f);
    void SetBackgroundColor(const Vector3& color);
    
    // 相机控制
    void SetCameraPosition(const Vector3& position);
    void SetCameraTarget(const Vector3& target);
    void SetCameraFOV(float fov);
    
    // 获取对象列表
    const std::vector<std::shared_ptr<SimpleObject>>& GetObjects() const { return m_objects; }
    const std::vector<std::shared_ptr<SimpleLight>>& GetLights() const { return m_lights; }
    
    // 更新场景（处理动画等）
    void Update(float deltaTime);
    
private:
    std::vector<std::shared_ptr<SimpleObject>> m_objects;
    std::vector<std::shared_ptr<SimpleLight>> m_lights;
    Vector3 m_ambientColor;
    float m_ambientIntensity;
    Vector3 m_backgroundColor;
    Vector3 m_cameraPosition;
    Vector3 m_cameraTarget;
    float m_cameraFOV;
    
    friend class SimpleRenderer;
};

/**
 * @brief 简单的渲染器
 */
class SimpleRenderer {
public:
    /**
     * @brief 渲染器配置
     */
    struct Config {
        int width = 800;
        int height = 600;
        std::string title = "AquaVisual Simple Renderer";
        bool fullscreen = false;
        bool vsync = true;
        bool enableValidation = false;
        
        Config() = default;
        Config(int w, int h, const std::string& t = "AquaVisual Simple Renderer")
            : width(w), height(h), title(t) {}
    };
    
    SimpleRenderer();
    ~SimpleRenderer();
    
    // 初始化和清理
    bool Initialize(const Config& config = Config());
    void Shutdown();
    
    // 渲染控制
    bool BeginFrame();
    void RenderScene(const SimpleScene& scene);
    void EndFrame();
    
    // 窗口控制
    bool ShouldClose() const;
    void PollEvents();
    
    // 实用方法
    bool IsInitialized() const { return m_initialized; }
    
    /**
     * @brief 一键渲染场景（简化版本）
     * @param scene 要渲染的场景
     * @param maxFrames 最大帧数，0表示无限制
     * @return 是否成功完成渲染
     */
    bool RenderSceneLoop(const SimpleScene& scene, int maxFrames = 0);
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
    bool m_initialized;
};

/**
 * @brief 便利函数：快速创建和显示一个简单场景
 */
namespace QuickStart {
    
    /**
     * @brief 创建一个包含基本光照的演示场景
     */
    std::unique_ptr<SimpleScene> CreateDemoScene();
    
    /**
     * @brief 快速显示场景
     * @param scene 要显示的场景
     * @param config 渲染器配置
     * @param duration 显示时长（秒），0表示直到用户关闭窗口
     */
    bool ShowScene(const SimpleScene& scene, 
                   const SimpleRenderer::Config& config = SimpleRenderer::Config(),
                   float duration = 0.0f);
    
    /**
     * @brief 一行代码显示演示场景
     */
    bool ShowDemoScene();
}

} // namespace Simple
} // namespace AquaVisual