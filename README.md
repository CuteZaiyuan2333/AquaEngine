# AquaEngine v1.0 - Texture System

一个现代化的3D游戏引擎，专注于高性能纹理系统和Vulkan渲染。

## 🌟 核心特性

- **完整纹理系统**: 支持多种图像格式的纹理加载和管理
- **Vulkan渲染引擎**: 高性能、现代化的图形API集成
- **实时纹理切换**: 支持运行时动态纹理切换
- **内存优化**: 智能内存管理和资源清理
- **跨平台**: 支持Windows、Linux、macOS

## 🎯 v1.0 版本亮点

- ✅ **完整纹理系统**: 从图像加载到Vulkan渲染的完整流程
- ✅ **STB_image集成**: 支持PNG、JPG、BMP、TGA等多种格式
- ✅ **Vulkan集成**: 纹理创建、内存分配、视图和采样器管理
- ✅ **交互式演示**: TextureDemo程序展示实时纹理切换
- ✅ **稳定性能**: 经过完整测试，稳定运行

## 🏗️ 项目架构

```
AquaEngine/
├── Core/                    # 核心引擎
│   ├── Renderer/           # Vulkan渲染引擎
│   ├── Math/               # 数学库
│   ├── Memory/             # 内存管理
│   └── Platform/           # 平台抽象层
├── Engine/                 # 游戏引擎层
│   ├── Scene/              # 场景管理
│   ├── Components/         # 组件系统
│   └── Systems/            # 系统架构
├── Editor/                 # AI集成编辑器
│   ├── UI/                 # 编辑器界面
│   ├── AI/                 # AI集成模块
│   └── Tools/              # 开发工具
├── Bindings/               # 语言绑定
│   ├── Python/             # Python绑定
│   └── Community/          # 社区语言支持
└── Examples/               # 示例项目
```

## 🚀 开发阶段

### 第一阶段：基础设施 ✅ 已完成
- [x] 项目结构初始化
- [x] 基础数学库
- [x] Vulkan初始化
- [x] 窗口创建

### 第二阶段：纹理系统 ✅ 已完成 (v1.0)
- [x] 纹理加载系统
- [x] STB_image集成
- [x] Vulkan纹理创建
- [x] 内存管理
- [x] 着色器集成
- [x] 实时演示程序

### 第三阶段：核心渲染 (下一版本)
- [ ] 完整渲染管线
- [ ] 3D模型加载
- [ ] 光照系统
- [ ] 相机系统

### 第四阶段：引擎核心 (未来版本)
- [ ] 场景图系统
- [ ] ECS架构
- [ ] 高级纹理功能

## 🛠️ 构建要求

- **C++20** 或更高版本
- **Vulkan SDK** 1.3+
- **CMake** 3.20+
- **Visual Studio 2022** (Windows)
- **GLFW** 3.3+ (窗口管理)

## 🚀 快速开始

### 1. 克隆项目
```bash
git clone https://github.com/CuteZaiyuan2333/AquaEngine.git
cd AquaEngine
```

### 2. 构建项目
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### 3. 运行演示
```bash
# 基础纹理测试
./build/Release/TextureTest.exe

# 交互式纹理演示
./build/Release/TextureDemo.exe
```

## 🎮 使用说明

### TextureDemo 交互控制
- **空格键**: 切换纹理（红色 ↔ 绿色）
- **ESC键**: 退出程序
- **鼠标**: 观察旋转的纹理四边形

## 📁 项目结构

```
AquaEngine/
├── Core/
│   ├── Texture.h/.cpp          # 纹理系统核心
│   └── VulkanContext.h/.cpp    # Vulkan上下文管理
├── Examples/
│   ├── TextureTest/            # 基础纹理测试
│   └── TextureDemo/            # 交互式演示
├── ThirdParty/
│   └── stb/                    # STB图像库
├── Assets/
│   ├── Textures/               # 纹理资源
│   └── Shaders/                # 着色器文件
└── Docs/                       # 完整文档
```

## 📝 文档

- [纹理系统状态](TEXTURE_SYSTEM_STATUS.md) - 详细技术文档
- [开发进展](TEXTURE_SYSTEM_PROGRESS.md) - 开发历程记录
- [发布说明](RELEASE_NOTES_v1.0.md) - v1.0版本详情
- [开发日志](DEVELOPMENT.md) - 开发过程记录

## 🤝 贡献

欢迎社区贡献！请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解详情。

## 📄 许可证

MIT License - 详见 [LICENSE](LICENSE) 文件