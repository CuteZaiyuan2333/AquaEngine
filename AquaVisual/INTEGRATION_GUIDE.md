# AquaVisual 集成指南

本指南详细说明如何将 AquaVisual 渲染引擎集成到您的项目中。

## 目录

- [快速开始](#快速开始)
- [CMake 集成](#cmake-集成)
- [手动集成](#手动集成)
- [API 使用](#api-使用)
- [集成示例](#集成示例)
- [常见问题](#常见问题)

## 快速开始

### 系统要求

- **操作系统**: Windows 10/11, Linux, macOS
- **编译器**: 支持 C++17 的编译器 (GCC 7+, Clang 5+, MSVC 2019+)
- **Vulkan**: Vulkan SDK 1.3+
- **CMake**: 3.16+

### 依赖项

AquaVisual 需要以下依赖项：

- **Vulkan SDK**: 图形 API
- **GLFW**: 窗口管理
- **GLM**: 数学库 (可选，AquaVisual 有内置数学库)

## CMake 集成

### 方法 1: 使用 find_package (推荐)

如果您已经安装了 AquaVisual：

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyProject)

# 查找 AquaVisual
find_package(AquaVisual REQUIRED)

# 创建您的可执行文件
add_executable(MyApp main.cpp)

# 链接 AquaVisual
target_link_libraries(MyApp PRIVATE AquaVisual::AquaVisual)
```

### 方法 2: 作为子项目

将 AquaVisual 作为子模块或子目录：

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyProject)

# 添加 AquaVisual 子目录
add_subdirectory(AquaVisual)

# 创建您的可执行文件
add_executable(MyApp main.cpp)

# 链接 AquaVisual
target_link_libraries(MyApp PRIVATE AquaVisual)
```

### 方法 3: 使用 FetchContent

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyProject)

include(FetchContent)

FetchContent_Declare(
    AquaVisual
    GIT_REPOSITORY https://github.com/yourusername/AquaEngine.git
    GIT_TAG        main
    SOURCE_SUBDIR  AquaVisual
)

FetchContent_MakeAvailable(AquaVisual)

add_executable(MyApp main.cpp)
target_link_libraries(MyApp PRIVATE AquaVisual)
```

## 手动集成

如果您不使用 CMake，可以手动集成：

### 1. 包含头文件

```cpp
#include "AquaVisual/AquaVisual.h"
```

### 2. 链接库

确保链接以下库：
- `AquaVisual` (静态或动态库)
- `vulkan-1` (Vulkan 库)
- `glfw3` (GLFW 库)

### 3. 编译器设置

- 启用 C++17: `-std=c++17`
- 包含目录: `-I/path/to/AquaVisual/Include`
- 库目录: `-L/path/to/AquaVisual/lib`

## API 使用

### 基本使用流程

```cpp
#include "AquaVisual/AquaVisual.h"
#include <iostream>

int main() {
    // 1. 初始化 AquaVisual
    if (!AquaVisual::Initialize()) {
        std::cerr << "Failed to initialize AquaVisual!" << std::endl;
        return -1;
    }
    
    // 2. 创建渲染器
    AquaVisual::RendererConfig config;
    config.width = 800;
    config.height = 600;
    config.title = "My Application";
    
    auto renderer = AquaVisual::CreateRenderer(config);
    if (!renderer || !renderer->Initialize()) {
        std::cerr << "Failed to create renderer!" << std::endl;
        return -1;
    }
    
    // 3. 主循环
    while (renderer->ShouldRender()) {
        renderer->SetClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        renderer->BeginFrame();
        
        // 在这里添加您的渲染代码
        
        renderer->EndFrame();
    }
    
    // 4. 清理
    renderer->Shutdown();
    AquaVisual::Shutdown();
    
    return 0;
}
```

### 渲染基本图形

```cpp
// 绘制三角形
AquaVisual::Vector3 v1(0.0f, 0.5f, 0.0f);
AquaVisual::Vector3 v2(-0.5f, -0.5f, 0.0f);
AquaVisual::Vector3 v3(0.5f, -0.5f, 0.0f);
renderer->DrawTriangle(v1, v2, v3);

// 加载并绘制纹理
auto texture = renderer->LoadTexture("path/to/texture.png");
if (texture) {
    AquaVisual::Vector2 position(100, 100);
    AquaVisual::Vector2 size(200, 200);
    renderer->DrawTexturedQuad(texture, position, size);
}
```

### 3D 渲染

```cpp
// 设置相机
AquaVisual::Matrix4 projection = AquaVisual::Matrix4::Perspective(
    AquaVisual::Math::ToRadians(45.0f), 
    800.0f / 600.0f, 
    0.1f, 
    100.0f
);
renderer->SetProjectionMatrix(projection);

AquaVisual::Matrix4 view = AquaVisual::Matrix4::LookAt(
    AquaVisual::Vector3(0, 0, 3),  // 相机位置
    AquaVisual::Vector3(0, 0, 0),  // 目标位置
    AquaVisual::Vector3(0, 1, 0)   // 上方向
);
renderer->SetViewMatrix(view);

// 创建并绘制网格
std::vector<float> vertices = { /* 顶点数据 */ };
std::vector<uint32_t> indices = { /* 索引数据 */ };
auto mesh = renderer->CreateMesh(vertices, indices);

AquaVisual::Transform transform;
transform.position = AquaVisual::Vector3(0, 0, 0);
transform.rotation = AquaVisual::Vector3(0, 0, 0);
transform.scale = AquaVisual::Vector3(1, 1, 1);

renderer->DrawMesh(mesh, transform);
```

## 集成示例

### 游戏引擎集成

参见 `Examples/Integration/AquaEngineIntegration.cpp`，展示了如何在游戏引擎中使用 AquaVisual。

### 第三方引擎集成

参见 `Examples/Integration/ThirdPartyIntegration.cpp`，展示了如何将 AquaVisual 集成到其他引擎中。

### 渲染后端抽象

```cpp
class IRenderBackend {
public:
    virtual ~IRenderBackend() = default;
    virtual bool Initialize() = 0;
    virtual void Render() = 0;
    virtual void Shutdown() = 0;
};

class AquaVisualBackend : public IRenderBackend {
private:
    std::unique_ptr<AquaVisual::RenderAPI> m_renderer;
    
public:
    bool Initialize() override {
        // 初始化 AquaVisual
        return true;
    }
    
    void Render() override {
        // 使用 AquaVisual 渲染
    }
    
    void Shutdown() override {
        // 清理 AquaVisual
    }
};
```

## 配置选项

### 编译时选项

在 CMake 中可以设置以下选项：

```cmake
# 构建选项
set(AQUAVISUAL_BUILD_EXAMPLES ON)  # 构建示例
set(AQUAVISUAL_BUILD_TESTS ON)    # 构建测试
set(AQUAVISUAL_BUILD_SHARED OFF)  # 构建共享库

# 功能选项
set(AQUAVISUAL_ENABLE_VALIDATION ON)  # 启用 Vulkan 验证层
set(AQUAVISUAL_ENABLE_PROFILING OFF)  # 启用性能分析
```

### 运行时配置

```cpp
AquaVisual::RendererConfig config;
config.width = 1024;
config.height = 768;
config.title = "My App";
config.enableValidation = true;   // 开发时启用
config.enableVSync = true;        // 启用垂直同步
config.msaaSamples = 4;          // MSAA 采样数
config.maxFramesInFlight = 2;    // 最大飞行帧数
```

## 性能优化

### 批量渲染

```cpp
// 批量绘制多个对象
std::vector<AquaVisual::Transform> transforms;
// ... 填充变换数据
renderer->DrawMeshInstanced(mesh, transforms);
```

### 资源管理

```cpp
// 预加载资源
auto textureManager = renderer->GetTextureManager();
textureManager->PreloadTexture("texture1.png");
textureManager->PreloadTexture("texture2.png");

// 使用对象池
auto meshPool = renderer->GetMeshPool();
auto mesh = meshPool->AcquireMesh();
// ... 使用网格
meshPool->ReleaseMesh(mesh);
```

## 常见问题

### Q: 如何处理窗口大小调整？

```cpp
renderer->SetResizeCallback([](uint32_t width, uint32_t height) {
    // 更新投影矩阵
    float aspect = static_cast<float>(width) / height;
    auto projection = AquaVisual::Matrix4::Perspective(
        AquaVisual::Math::ToRadians(45.0f), aspect, 0.1f, 100.0f
    );
    renderer->SetProjectionMatrix(projection);
});
```

### Q: 如何处理输入事件？

```cpp
renderer->SetKeyCallback([](int key, int scancode, int action, int mods) {
    if (key == 256 && action == 1) { // ESC 键
        // 处理退出
    }
});

renderer->SetMouseCallback([](double xpos, double ypos) {
    // 处理鼠标移动
});
```

### Q: 如何启用调试信息？

```cpp
// 编译时启用验证层
AquaVisual::RendererConfig config;
config.enableValidation = true;

// 运行时获取调试信息
std::cout << "Renderer: " << renderer->GetRendererName() << std::endl;
std::cout << "GPU: " << renderer->GetGPUInfo() << std::endl;
std::cout << "FPS: " << renderer->GetFPS() << std::endl;
```

### Q: 如何处理错误？

```cpp
try {
    auto renderer = AquaVisual::CreateRenderer(config);
    if (!renderer) {
        throw std::runtime_error("Failed to create renderer");
    }
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}
```

## 更多资源

- [API 参考文档](API_REFERENCE.md)
- [示例项目](Examples/)
- [性能指南](PERFORMANCE_GUIDE.md)
- [故障排除](TROUBLESHOOTING.md)

## 支持

如果您在集成过程中遇到问题，请：

1. 查看 [常见问题](#常见问题) 部分
2. 查看示例代码
3. 提交 Issue 到 GitHub 仓库
4. 联系开发团队

---

**注意**: 本指南假设您已经熟悉 C++ 和基本的图形编程概念。如果您是图形编程新手，建议先学习相关基础知识。