# AquaEngine 调试项目集合

这个文件夹包含了使用 AquaVisual 渲染引擎的各种调试和演示项目。每个项目都是独立的，用于测试和展示 AquaVisual 的不同功能。

## 📁 项目列表

### 🎮 RotatingCube - 旋转彩色立方体
- **功能**: 展示一个在三个轴上旋转的彩色立方体
- **特性**: 多轴旋转动画、彩色顶点、高性能渲染
- **技术点**: 顶点颜色、变换矩阵、时间动画
- **状态**: ✅ 完成

## 🎯 项目目标

这些调试项目的主要目标是：

1. **功能验证**: 验证 AquaVisual 各项功能的正确性
2. **性能测试**: 测试渲染引擎在不同场景下的性能
3. **使用示例**: 为开发者提供使用 AquaVisual 的实际例子
4. **问题调试**: 快速创建最小化的测试用例来定位问题

## 🔧 通用构建要求

所有项目都需要以下前置条件：

### 必需组件
- **AquaVisual 库**: 已构建的 AquaVisual 渲染引擎
- **Vulkan SDK**: 版本 1.3 或更高
- **CMake**: 版本 3.16 或更高
- **Visual Studio**: 2019 或更高版本

### 依赖路径
项目假设以下目录结构：
```
AquaEngine/
├── AquaVisual/                    # AquaVisual 源码
├── ThirdParty/GLFW/              # GLFW 库
├── build_window_demo/            # AquaVisual 构建输出
└── DebugProjects/                # 调试项目（当前目录）
    ├── RotatingCube/
    └── [其他项目]/
```

## 🚀 快速开始

### 1. 确保 AquaVisual 已构建
```bash
cd ../../
cmake -B build_window_demo -DCMAKE_BUILD_TYPE=Debug
cmake --build build_window_demo --config Debug
```

### 2. 选择并构建项目
```bash
cd DebugProjects/RotatingCube
build_and_run.bat
```

## 📊 项目特性对比

| 项目 | 几何体 | 动画 | 纹理 | 光照 | 复杂度 |
|------|--------|------|------|------|--------|
| RotatingCube | 立方体 | ✅ 旋转 | ❌ | ❌ | 简单 |

## 🔮 未来项目计划

### 即将添加的项目
- **TexturedCube**: 带纹理的立方体
- **MultipleObjects**: 多个对象渲染
- **LightingDemo**: 基础光照演示
- **ParticleSystem**: 粒子系统演示
- **ModelLoader**: 模型加载演示

### 高级项目
- **ShadowMapping**: 阴影映射
- **PostProcessing**: 后处理效果
- **TerrainRenderer**: 地形渲染
- **WaterSimulation**: 水面模拟

## 🛠️ 开发指南

### 创建新项目
1. 在 `DebugProjects` 下创建新文件夹
2. 复制现有项目的 `CMakeLists.txt` 作为模板
3. 修改项目名称和源文件
4. 创建构建脚本和说明文档

### 项目结构模板
```
NewProject/
├── main.cpp                 # 主程序
├── CMakeLists.txt          # 构建配置
├── build_and_run.bat       # 构建脚本
└── README.md              # 项目说明
```

### 编码规范
- 使用 AquaVisual 命名空间
- 遵循 RAII 原则进行资源管理
- 添加适当的错误处理
- 包含性能监控代码

## 🐛 故障排除

### 通用问题
1. **库未找到**: 确保 AquaVisual 已正确构建
2. **Vulkan 错误**: 检查 Vulkan SDK 安装和驱动
3. **着色器问题**: 确保着色器文件已编译并复制

### 调试技巧
- 使用 Visual Studio 调试器
- 检查控制台输出信息
- 验证文件路径和依赖
- 使用 Vulkan 验证层

## 📚 相关文档

- [AquaVisual API 参考](../AquaVisual/API_REFERENCE.md)
- [AquaVisual 快速开始](../AquaVisual/QUICK_START.md)
- [项目状态总结](../PROJECT_STATUS.md)
- [开发历程](../DEVELOPMENT_JOURNEY.md)

---

**注意**: 这些项目主要用于开发和调试目的。对于生产环境的应用，请参考 AquaVisual 的完整文档和最佳实践指南。