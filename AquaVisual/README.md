# AquaVisual Rendering Engine - MVP Version

AquaVisual MVP (v0.1) 是一个基于 Vulkan 的简化渲染引擎，专注于提供基础的 3D 渲染功能。这是 AquaVisual 的最小可行产品版本，旨在快速验证核心架构并提供基础渲染能力。

## 🎯 MVP 目标

- **基础渲染**: 支持基本几何体渲染 (三角形、立方体、平面)
- **纹理映射**: 简单的纹理加载和应用
- **相机系统**: 基础的3D相机控制
- **简单光照**: 基本的漫反射光照模型
- **易于使用**: 提供简洁的API，快速上手

## ✨ MVP 特性

### 🚀 基础渲染
- **Vulkan 后端** - 基于现代 Vulkan API
- **基础几何体** - 三角形、立方体、平面渲染
- **纹理支持** - 基础纹理加载和映射
- **简单光照** - 漫反射光照模型

### 🎨 资源系统
- **网格管理** - 基础几何体生成和管理
- **纹理加载** - 支持常见图像格式
- **着色器系统** - 基础顶点和片段着色器

### 🔧 开发工具
- **简洁 API** - 易于学习和使用的接口
- **示例程序** - 完整的使用示例
- **错误处理** - 基础的错误检查和报告

## 🏗️ 架构设计

### 核心模块
```
AquaVisual/
├── Core/                   # 渲染核心
│   ├── RenderAPI.h        # 渲染API抽象接口
│   ├── VulkanRenderer.h   # Vulkan实现
│   ├── Texture.h          # 纹理系统
│   ├── Shader.h           # 着色器系统
│   └── Mesh.h             # 网格系统
├── Math/                   # 数学库
│   ├── Vector.h           # 向量运算
│   ├── Matrix.h           # 矩阵运算
│   └── Transform.h        # 变换系统
├── Platform/               # 平台抽象
│   ├── Window.h           # 窗口接口
│   └── Input.h            # 输入接口
└── Utils/                  # 工具类
    ├── Logger.h           # 日志系统
    └── Config.h           # 配置管理
```

### API设计原则
- **最小依赖** - 只依赖必要的系统库
- **清晰接口** - 简单直观的API设计
- **错误处理** - 完善的错误处理和调试支持
- **内存安全** - RAII和智能指针管理资源

## 🚀 快速开始

### 基础使用示例
```cpp
#include "AquaVisual/AquaVisual.h"

int main() {
    // 创建渲染器
    auto renderer = AquaVisual::CreateRenderer({
        .width = 800,
        .height = 600,
        .title = "My App"
    });
    
    // 加载纹理
    auto texture = renderer->LoadTexture("texture.png");
    
    // 渲染循环
    while (renderer->ShouldRender()) {
        renderer->BeginFrame();
        
        // 渲染纹理四边形
        renderer->DrawTexturedQuad(texture, {0, 0}, {100, 100});
        
        renderer->EndFrame();
    }
    
    return 0;
}
```

### 集成到游戏引擎
```cpp
// 在你的游戏引擎中
class MyGameEngine {
private:
    std::unique_ptr<AquaVisual::Renderer> m_renderer;
    
public:
    void Initialize() {
        // 创建AquaVisual渲染器
        m_renderer = AquaVisual::CreateRenderer(config);
    }
    
    void Render() {
        m_renderer->BeginFrame();
        
        // 渲染游戏对象
        for (auto& object : gameObjects) {
            m_renderer->DrawMesh(object.mesh, object.transform);
        }
        
        m_renderer->EndFrame();
    }
};
```

## 📦 集成指南

### CMake集成
```cmake
# 添加AquaVisual子目录
add_subdirectory(AquaVisual)

# 链接到你的项目
target_link_libraries(YourProject PRIVATE AquaVisual)
```

### 包管理器支持
- **vcpkg** - `vcpkg install aquavisual`
- **Conan** - `conan install aquavisual/1.0@`
- **手动集成** - 复制头文件和库文件

## 🎮 示例项目

### 1. HelloTriangle
最基础的渲染示例，展示如何使用AquaVisual渲染一个三角形。

### 2. TextureDemo
纹理渲染演示，展示纹理加载和渲染功能。

### 3. 3DViewer
3D模型查看器，展示复杂网格渲染和相机控制。

## 🔧 构建要求

### 系统要求
- **操作系统**: Windows 10+ / Linux / macOS
- **编译器**: MSVC 2019+ / GCC 9+ / Clang 10+
- **CMake**: 3.20+
- **Vulkan SDK**: 1.2+

### 依赖库
- **GLFW** - 窗口和输入管理
- **STB** - 图像加载
- **GLM** - 数学库（可选，有内置数学库）

## 📊 性能特点

- **低延迟** - 直接GPU控制，最小化驱动开销
- **高吞吐** - 批处理渲染，减少绘制调用
- **内存高效** - 智能缓冲区管理和内存池
- **多线程** - 支持多线程命令缓冲区录制

## 🌐 平台支持

| 平台 | 状态 | 备注 |
|------|------|------|
| Windows | ✅ 完全支持 | 主要开发平台 |
| Linux | 🔄 开发中 | 计划v1.1支持 |
| macOS | 📋 计划中 | 通过MoltenVK支持 |
| Android | 📋 计划中 | 移动平台支持 |

## 🔮 发展路线图

### v1.0 (当前) - 基础渲染
- [x] Vulkan渲染管线
- [x] 基础几何渲染
- [x] 纹理系统
- [x] 着色器系统

### v1.1 - 高级渲染
- [ ] PBR材质系统
- [ ] 阴影映射
- [ ] 后处理效果
- [ ] Linux平台支持

### v1.2 - 性能优化
- [ ] 多线程渲染
- [ ] GPU Culling
- [ ] 延迟渲染
- [ ] 计算着色器

### v2.0 - 高级特性
- [ ] 光线追踪支持
- [ ] 体积渲染
- [ ] GPU粒子系统
- [ ] 跨平台完整支持

## 🤝 社区和贡献

### 使用AquaVisual的项目
- **AquaEngine** - 原生游戏引擎
- **YourProject** - 欢迎提交你的项目

### 贡献指南
我们欢迎社区贡献！请遵循以下指南：
- 专注于渲染相关功能
- 保持API的简洁性和一致性
- 添加适当的测试和文档
- 遵循现有的代码风格

### 移植到其他引擎
AquaVisual设计为易于集成，已知的集成案例：
- **Bevy** (Rust) - 通过C FFI绑定
- **Godot** - 通过GDNative
- **Unity** - 通过Native Plugin

## 📄 许可证

MIT License - 详见 [LICENSE](LICENSE) 文件

## 📚 文档

- [集成指南](INTEGRATION_GUIDE.md) - 详细的集成说明和使用方法
- [API 参考](API_REFERENCE.md) - 完整的 API 文档和函数说明
- [示例项目](Examples/) - 各种使用示例和集成案例
  - [集成示例](Examples/Integration/) - 展示如何集成到不同引擎
- [CMake 配置](AquaVisualConfig.cmake.in) - CMake 包配置模板

## 📞 联系我们

- **GitHub**: [AquaVisual Repository](https://github.com/your-org/aquavisual)
- **文档**: [AquaVisual Docs](https://aquavisual.dev)
- **社区**: [Discord Server](https://discord.gg/aquavisual)

---

**AquaVisual v1.0** - 让渲染变得简单而强大 🌊✨