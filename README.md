# AquaEngine - 现代化游戏引擎套件

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)
![Status](https://img.shields.io/badge/status-MVP%20Complete%20%26%20Stable-brightgreen.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)

AquaEngine 是一个现代化的游戏引擎套件，由两个核心组件构成：

- **AquaVisual** - 高性能 Vulkan 渲染引擎 ✅ **MVP 完成**
- **AquaCore** - 基于 AquaVisual 的完整游戏引擎（规划中）

## 🎯 项目愿景

AquaEngine 旨在提供一个模块化、高性能的游戏开发解决方案，让开发者能够：

- 使用 AquaVisual 进行高质量的 3D 渲染
- 通过 AquaCore 快速构建完整的游戏项目
- 享受现代 C++ 和 Vulkan 带来的性能优势

## 📦 组件概览

### AquaVisual - 渲染引擎 ✅
- **状态**: ✅ **MVP 完成并成功运行**
- **技术栈**: Vulkan, C++17, GLFW
- **特性**: 
  - ✅ 基础 3D 渲染（三角形、立方体、球体、平面）
  - ✅ 完整的数学库（Vector3, Matrix4）
  - ✅ 相机系统（透视投影、视图变换）
  - ✅ 纹理系统基础
  - ✅ Vulkan 渲染管线
  - ✅ 高性能渲染循环（12000+ FPS）
- **目标**: 提供简洁易用的渲染 API

### AquaCore - 游戏引擎 📋
- **状态**: 📋 **规划中**
- **依赖**: AquaVisual
- **特性**: 场景管理、实体组件系统、物理集成、音频系统
- **目标**: 完整的游戏开发框架

## 🚀 快速开始

### 前置要求

- **编译器**: Visual Studio 2019+ (Windows) 或 GCC 9+ (Linux)
- **CMake**: 3.20+
- **Vulkan SDK**: 1.3+
- **依赖库**: GLFW, GLM

### 构建项目

```bash
# 克隆项目
git clone https://github.com/your-username/AquaEngine.git
cd AquaEngine

# 创建构建目录
mkdir build && cd build

# 配置项目（只构建 AquaVisual）
cmake .. -DAQUA_BUILD_VISUAL=ON -DAQUA_BUILD_CORE=OFF

# 编译
cmake --build . --config Release
```

### 运行示例

```bash
# 运行基础三角形示例
./bin/BasicTriangle
```

## 📁 项目结构

```
AquaEngine/
├── AquaVisual/              # 渲染引擎
│   ├── Include/             # 公共头文件
│   ├── Source/              # 实现代码
│   ├── Shaders/             # 着色器文件
│   └── Examples/            # 示例程序
├── AquaCore/                # 游戏引擎（未来）
│   ├── Include/             # 公共头文件
│   ├── Source/              # 实现代码
│   └── Examples/            # 示例游戏
├── CMakeLists.txt           # 根构建配置
└── README.md                # 项目说明
```

## 🛠️ 开发状态

### AquaVisual MVP (v0.1) - ✅ 完成并稳定运行
- [x] 项目架构设计
- [x] 核心 API 接口定义
- [x] Vulkan 渲染器框架
- [x] **基础渲染管线实现** ✅ **完成**
- [x] **网格和纹理系统** ✅ **完成**
- [x] **相机控制系统** ✅ **完成**
- [x] **示例程序完善** ✅ **完成**
- [x] **成功渲染彩色三角形和立方体** ✅ **完成**
- [x] **构建系统完善** ✅ **新增完成**
- [x] **GLFW链接问题解决** ✅ **新增完成**

### 🎉 MVP 成功演示
- ✅ **CubeWindowDemo** - 成功运行，显示旋转的彩色立方体
- ✅ **面剔除修复** - 正确的顶点缠绕顺序和面剔除配置
- ✅ **深度测试配置** - 完整的3D渲染管线
- ✅ **高性能渲染** - 稳定的渲染循环
- ✅ **完整的 Vulkan 管线** - 从初始化到渲染输出
- ✅ **数学库验证** - 相机变换、投影矩阵、旋转动画正常工作
- ✅ **构建系统稳定** - Visual Studio 2022 + CMake 完全兼容
- ✅ **依赖管理完善** - GLFW、Vulkan SDK 正确集成

### 🔧 最近解决的技术问题
- ✅ **LNK1104 链接器错误** - 成功解决 GLFW 库链接问题
- ✅ **CMake 配置优化** - 直接指定库路径，避免 find_package 问题
- ✅ **Visual Studio 项目文件修复** - 移除硬编码的错误库引用
- ✅ **构建流程标准化** - 确保项目可以在不同环境下稳定构建

### AquaCore (v1.0) - 📋 规划中
- [ ] 场景管理系统
- [ ] 实体组件系统 (ECS)
- [ ] 物理引擎集成
- [ ] 音频系统
- [ ] 输入管理
- [ ] 资源管理器

## 📚 文档

### AquaVisual 文档
- [AquaVisual 快速开始](./AquaVisual/QUICK_START.md)
- [AquaVisual API 参考](./AquaVisual/API_REFERENCE.md)
- [架构设计文档](./AquaVisual/ARCHITECTURE_DESIGN.md)
- [开发状态](./AquaVisual/DEVELOPMENT_STATUS.md)

### 项目文档
- [项目状态总结](./PROJECT_STATUS.md) - 当前项目状态和里程碑
- [AquaCore 规划](./AquaCore/README.md) - 游戏引擎开发计划

## 🤝 贡献

我们欢迎社区贡献！AquaVisual MVP 已完成，现在是参与 AquaCore 开发的好时机。

## 📄 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🔗 相关链接

- [Vulkan 官方文档](https://vulkan.lunarg.com/)
- [GLFW 文档](https://www.glfw.org/documentation.html)
- [GLM 文档](https://glm.g-truc.net/)

---

**注意**: AquaEngine 目前处于早期开发阶段。AquaVisual 正在积极开发中，AquaCore 将在 AquaVisual 稳定后开始开发。