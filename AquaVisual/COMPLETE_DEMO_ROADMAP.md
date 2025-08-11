# 🎯 AquaVisual 完整Demo开发路线图

## 📋 目标：实现带光照和FBX加载的完整Demo

### 🎮 最终Demo功能目标
- ✅ **基础渲染**：立方体、球体等基础几何体
- 🔄 **光照系统**：Phong/PBR光照模型，多光源支持
- 📦 **FBX模型加载**：支持复杂3D模型导入
- 🎨 **材质系统**：纹理、法线贴图、PBR材质
- 🎬 **相机控制**：自由视角、轨道相机
- 🎭 **动画系统**：模型动画、光源动画

---

## 📊 当前状态分析

### ✅ 已完成的核心功能
1. **Vulkan渲染器** - 基础渲染管线工作正常
2. **数学库** - Vector3, Matrix4 完整实现
3. **基础几何体** - 立方体、球体、平面生成
4. **简单着色器** - 基础顶点/片段着色器
5. **窗口系统** - GLFW集成，事件处理

### 🔄 部分完成的功能
1. **光照系统** - 有基础框架，需要完善
2. **材质系统** - 基础结构存在，需要扩展
3. **纹理系统** - 基础加载，需要完善格式支持

### ❌ 缺失的关键功能
1. **FBX文件加载** - 完全缺失
2. **高级光照** - PBR材质，阴影
3. **动画系统** - 骨骼动画，关键帧动画
4. **资源管理** - 统一的资源加载和缓存

---

## 🗺️ 开发路线图

### 阶段1：完善光照系统 (1-2周)

#### 1.1 扩展光照着色器
```glsl
// 目标：实现完整的Phong光照模型
// 文件：Shaders/phong_lighting.frag

#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;

// 光源结构
struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
};

// 材质参数
layout(binding = 1) uniform MaterialUBO {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
} material;

// 光源数据
layout(binding = 2) uniform LightingUBO {
    DirectionalLight dirLight;
    PointLight pointLights[8];
    int numPointLights;
    vec3 viewPos;
} lighting;

// 纹理
layout(binding = 3) uniform sampler2D albedoTexture;
layout(binding = 4) uniform sampler2D normalTexture;
layout(binding = 5) uniform sampler2D metallicRoughnessTexture;

void main() {
    // 实现完整的PBR光照计算
    // ...
}
```

#### 1.2 光照系统C++实现
```cpp
// 文件：Include/AquaVisual/Lighting/LightingSystem.h

namespace AquaVisual::Lighting {
    struct DirectionalLight {
        Vector3 direction;
        Vector3 color;
        float intensity = 1.0f;
    };
    
    struct PointLight {
        Vector3 position;
        Vector3 color;
        float intensity = 1.0f;
        float radius = 10.0f;
    };
    
    class LightingSystem {
    public:
        void AddDirectionalLight(const DirectionalLight& light);
        void AddPointLight(const PointLight& light);
        void RemoveLight(uint32_t lightId);
        
        void UpdateLightingUBO();
        void BindLightingData(VkCommandBuffer commandBuffer);
        
        // 阴影映射
        void EnableShadows(bool enable);
        void UpdateShadowMaps();
        
    private:
        std::vector<DirectionalLight> m_directionalLights;
        std::vector<PointLight> m_pointLights;
        
        // Vulkan资源
        VkBuffer m_lightingUBO;
        VkDeviceMemory m_lightingUBOMemory;
        VkDescriptorSet m_lightingDescriptorSet;
    };
}
```

#### 1.3 任务清单
- [ ] 创建 `LightingSystem.h` 和 `LightingSystem.cpp`
- [ ] 实现多光源支持（最多8个点光源 + 1个方向光）
- [ ] 创建光照专用的UBO（Uniform Buffer Object）
- [ ] 更新着色器支持多光源计算
- [ ] 集成到现有的渲染管线中

---

### 阶段2：实现FBX文件加载 (2-3周)

#### 2.1 选择FBX加载库
**推荐方案：Assimp库**
- 支持40+种3D文件格式（包括FBX）
- 开源，文档完善
- 与Vulkan兼容性好

#### 2.2 集成Assimp
```cmake
# CMakeLists.txt 添加
find_package(assimp REQUIRED)
target_link_libraries(AquaVisual PRIVATE assimp::assimp)
```

#### 2.3 模型加载器实现
```cpp
// 文件：Include/AquaVisual/Resources/ModelLoader.h

namespace AquaVisual::Resources {
    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector2 texCoord;
        Vector3 tangent;
        Vector3 bitangent;
    };
    
    struct SubMesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        uint32_t materialIndex;
    };
    
    struct Model {
        std::vector<SubMesh> subMeshes;
        std::vector<Material> materials;
        std::string directory;
    };
    
    class ModelLoader {
    public:
        static std::shared_ptr<Model> LoadModel(const std::string& path);
        static std::shared_ptr<Model> LoadFBX(const std::string& path);
        
    private:
        static SubMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
        static Material LoadMaterial(aiMaterial* mat, const std::string& directory);
        static std::shared_ptr<Texture> LoadTexture(const std::string& path);
    };
}
```

#### 2.4 任务清单
- [ ] 集成Assimp库到项目中
- [ ] 实现 `ModelLoader` 类
- [ ] 支持FBX、OBJ、GLTF格式
- [ ] 实现材质和纹理自动加载
- [ ] 创建模型渲染示例

---

### 阶段3：完善材质系统 (1-2周)

#### 3.1 PBR材质实现
```cpp
// 文件：Include/AquaVisual/Materials/PBRMaterial.h

namespace AquaVisual::Materials {
    class PBRMaterial {
    public:
        // 基础属性
        void SetAlbedo(const Vector3& color);
        void SetMetallic(float metallic);
        void SetRoughness(float roughness);
        void SetAO(float ao);
        
        // 纹理
        void SetAlbedoTexture(std::shared_ptr<Texture> texture);
        void SetNormalTexture(std::shared_ptr<Texture> texture);
        void SetMetallicRoughnessTexture(std::shared_ptr<Texture> texture);
        void SetAOTexture(std::shared_ptr<Texture> texture);
        
        // 渲染
        void Bind(VkCommandBuffer commandBuffer);
        void UpdateUBO();
        
    private:
        Vector3 m_albedo = {1.0f, 1.0f, 1.0f};
        float m_metallic = 0.0f;
        float m_roughness = 0.5f;
        float m_ao = 1.0f;
        
        std::shared_ptr<Texture> m_albedoTexture;
        std::shared_ptr<Texture> m_normalTexture;
        std::shared_ptr<Texture> m_metallicRoughnessTexture;
        std::shared_ptr<Texture> m_aoTexture;
    };
}
```

#### 3.2 任务清单
- [ ] 实现 `PBRMaterial` 类
- [ ] 创建材质编辑器（可选）
- [ ] 支持多种纹理格式（PNG, JPG, TGA, DDS）
- [ ] 实现材质预设系统

---

### 阶段4：相机控制系统 (1周)

#### 4.1 相机控制器
```cpp
// 文件：Include/AquaVisual/Camera/CameraController.h

namespace AquaVisual::Camera {
    class OrbitCameraController {
    public:
        void Update(float deltaTime);
        void HandleInput(const InputState& input);
        
        void SetTarget(const Vector3& target);
        void SetDistance(float distance);
        void SetAngles(float yaw, float pitch);
        
        Matrix4 GetViewMatrix() const;
        Matrix4 GetProjectionMatrix() const;
        
    private:
        Vector3 m_target = {0, 0, 0};
        float m_distance = 5.0f;
        float m_yaw = 0.0f;
        float m_pitch = 0.0f;
        
        float m_fov = 45.0f;
        float m_aspectRatio = 16.0f / 9.0f;
        float m_nearPlane = 0.1f;
        float m_farPlane = 100.0f;
    };
    
    class FreeCameraController {
    public:
        void Update(float deltaTime);
        void HandleInput(const InputState& input);
        
        Matrix4 GetViewMatrix() const;
        Matrix4 GetProjectionMatrix() const;
        
    private:
        Vector3 m_position = {0, 0, 5};
        Vector3 m_forward = {0, 0, -1};
        Vector3 m_up = {0, 1, 0};
        
        float m_moveSpeed = 5.0f;
        float m_mouseSensitivity = 0.1f;
    };
}
```

#### 4.2 任务清单
- [ ] 实现轨道相机控制器
- [ ] 实现自由相机控制器
- [ ] 添加鼠标和键盘输入处理
- [ ] 实现相机动画系统

---

### 阶段5：创建完整Demo (1周)

#### 5.1 Demo场景设计
```cpp
// 文件：Examples/CompleteDemo/CompleteDemo.cpp

class CompleteDemo {
public:
    bool Initialize();
    void Run();
    void Cleanup();
    
private:
    void LoadAssets();
    void SetupScene();
    void SetupLighting();
    void Update(float deltaTime);
    void Render();
    void HandleInput();
    
    // 渲染器
    std::unique_ptr<VulkanRenderer> m_renderer;
    
    // 场景对象
    std::shared_ptr<Model> m_dragonModel;
    std::shared_ptr<Model> m_environmentModel;
    
    // 光照
    std::unique_ptr<LightingSystem> m_lightingSystem;
    
    // 相机
    std::unique_ptr<OrbitCameraController> m_camera;
    
    // 材质
    std::vector<std::shared_ptr<PBRMaterial>> m_materials;
};
```

#### 5.2 Demo功能特性
1. **场景内容**：
   - 加载复杂FBX模型（如龙、建筑等）
   - 环境几何体（地面、天空盒）
   - 多个材质展示

2. **光照效果**：
   - 1个主方向光（太阳光）
   - 3-4个点光源（环境光）
   - 实时阴影（可选）

3. **交互功能**：
   - 鼠标轨道相机控制
   - 键盘切换光照模式
   - 实时调整材质参数

4. **性能展示**：
   - FPS计数器
   - 渲染统计信息
   - 内存使用情况

#### 5.3 任务清单
- [ ] 创建 `CompleteDemo` 项目
- [ ] 准备测试资源（FBX模型、纹理）
- [ ] 实现完整的渲染循环
- [ ] 添加UI界面（ImGui集成）
- [ ] 性能优化和调试

---

## 📦 所需第三方库

### 必需库
1. **Assimp** - 3D模型加载
   ```bash
   vcpkg install assimp:x64-windows
   ```

2. **stb_image** - 图像加载（已有）
   ```bash
   vcpkg install stb:x64-windows
   ```

3. **ImGui** - 调试UI（可选）
   ```bash
   vcpkg install imgui[vulkan-binding]:x64-windows
   ```

### 可选库
1. **PhysX** - 物理引擎（未来扩展）
2. **OpenAL** - 音频系统（未来扩展）

---

## 📈 开发时间估算

| 阶段 | 功能 | 预计时间 | 优先级 |
|------|------|----------|--------|
| 1 | 完善光照系统 | 1-2周 | 🔴 高 |
| 2 | FBX文件加载 | 2-3周 | 🔴 高 |
| 3 | 完善材质系统 | 1-2周 | 🟡 中 |
| 4 | 相机控制系统 | 1周 | 🟡 中 |
| 5 | 创建完整Demo | 1周 | 🟢 低 |

**总计：6-9周**

---

## 🎯 里程碑检查点

### 里程碑1：基础光照 (2周后)
- [ ] 多光源渲染正常工作
- [ ] Phong光照模型完整实现
- [ ] 光照参数可实时调整

### 里程碑2：模型加载 (4-5周后)
- [ ] 成功加载FBX文件
- [ ] 复杂模型正常渲染
- [ ] 材质和纹理自动加载

### 里程碑3：完整Demo (6-9周后)
- [ ] 所有功能集成完成
- [ ] 性能达到60+ FPS
- [ ] 用户交互流畅

---

## 🚀 快速开始

### 立即可以开始的任务
1. **扩展光照着色器** - 修改现有的 `cube_lit.frag`
2. **创建LightingSystem类** - 基于现有的Light结构
3. **集成Assimp库** - 修改CMakeLists.txt

### 第一个目标
**在2周内实现一个支持多光源的立方体场景**，为后续的FBX加载打下基础。

---

这个路线图提供了清晰的开发路径，每个阶段都有具体的代码示例和任务清单。你想从哪个阶段开始？我可以帮你实现具体的功能。