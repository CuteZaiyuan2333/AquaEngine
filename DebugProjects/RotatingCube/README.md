# 旋转彩色立方体演示

这是一个使用 AquaVisual 渲染引擎的独立演示程序，展示了一个旋转的彩色立方体。

## 🎯 功能特性

- **彩色立方体**: 每个面都有不同的颜色
- **平滑旋转**: 立方体在三个轴上同时旋转
- **高性能渲染**: 基于 Vulkan 的高效渲染
- **实时动画**: 60+ FPS 的流畅动画

## 🎨 立方体颜色方案

- **前面**: 红色 (Red)
- **后面**: 绿色 (Green)  
- **左面**: 蓝色 (Blue)
- **右面**: 黄色 (Yellow)
- **上面**: 品红色 (Magenta)
- **下面**: 青色 (Cyan)

## 🔧 构建要求

### 前置条件
1. **AquaVisual 已构建**: 确保 AquaVisual 库已经成功构建
2. **Vulkan SDK**: 已安装 Vulkan SDK
3. **CMake**: 版本 3.16 或更高
4. **Visual Studio**: 2019 或更高版本

### 依赖检查
在构建之前，请确保以下文件存在：
```
../../build_window_demo/AquaVisual/Debug/AquaVisual.lib
../../ThirdParty/GLFW/lib-vc2022/glfw3.lib
../../AquaVisual/Shaders/*.spv
```

## 🚀 快速开始

### 方法 1: 使用构建脚本（推荐）
```bash
# 双击运行构建脚本
build_and_run.bat
```

### 方法 2: 手动构建
```bash
# 配置项目
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# 构建项目
cmake --build build --config Debug

# 运行程序
cd build/Debug
RotatingCubeDemo.exe
```

## 🎮 控制说明

- **ESC**: 退出程序
- **窗口关闭按钮**: 退出程序

## 📁 项目结构

```
RotatingCube/
├── rotating_cube_demo.cpp    # 主程序源码
├── CMakeLists.txt           # CMake 构建配置
├── build_and_run.bat        # 构建和运行脚本
├── README.md               # 项目说明文档
└── build/                  # 构建输出目录
    └── Debug/
        ├── RotatingCubeDemo.exe
        └── Shaders/        # 着色器文件
```

## 🔍 技术实现

### 核心特性
- **顶点数据**: 24个顶点，每个顶点包含位置和颜色信息
- **索引缓冲**: 36个索引，定义12个三角形（6个面 × 2个三角形）
- **动画系统**: 基于时间的旋转动画
- **相机系统**: 透视投影相机，观察距离5个单位

### 旋转动画
```cpp
// 多轴旋转，创造有趣的视觉效果
rotationX = rotationAngle * 0.5f  // X轴慢速旋转
rotationY = rotationAngle * 1.0f  // Y轴正常速度
rotationZ = rotationAngle * 0.3f  // Z轴更慢旋转
```

### 渲染流程
1. **初始化**: 创建渲染器和立方体网格
2. **更新**: 计算旋转角度和变换矩阵
3. **渲染**: 应用变换并渲染立方体
4. **清理**: 释放资源

## 🐛 故障排除

### 常见问题

1. **"AquaVisual library not found"**
   - 确保先构建了 AquaVisual 项目
   - 检查路径 `../../build_window_demo/AquaVisual/Debug/AquaVisual.lib`

2. **"GLFW library not found"**
   - 检查 GLFW 库路径是否正确
   - 确保使用了正确的 Visual Studio 版本对应的库

3. **着色器错误**
   - 确保着色器文件已编译（.spv 文件）
   - 检查着色器文件是否正确复制到输出目录

4. **Vulkan 初始化失败**
   - 确保已安装 Vulkan SDK
   - 检查显卡驱动是否支持 Vulkan

### 调试信息
程序运行时会输出详细的调试信息，包括：
- 初始化状态
- 渲染器创建结果
- 帧率信息
- 错误消息

## 📈 性能指标

- **目标帧率**: 60+ FPS
- **顶点数**: 24个顶点
- **三角形数**: 12个三角形
- **内存使用**: 最小化内存占用

## 🔗 相关项目

- **AquaVisual**: 核心渲染引擎
- **CubeWindowDemo**: 基础立方体演示
- **AquaCore**: 未来的游戏引擎（规划中）

---

**注意**: 这是一个演示程序，展示了 AquaVisual 渲染引擎的基本功能。更多高级功能请参考 AquaVisual 的完整文档。