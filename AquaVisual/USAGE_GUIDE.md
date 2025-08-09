# AquaVisual 使用指南

## 概述

AquaVisual 是一个轻量级的 C++ 图形渲染库，提供了基础的数学运算、几何体生成、网格操作和纹理系统功能。本指南将介绍如何使用 AquaVisual 库。

## 当前功能

### 1. 数学库
- **向量运算**: Vector2, Vector3, Vector4
- **矩阵运算**: Matrix4 (单位矩阵、矩阵乘法)
- **基础运算**: 加法、减法、点积、叉积、长度计算、归一化

### 2. 几何体生成
- **基础图形**: 三角形、四边形、立方体、球体、平面
- **可配置参数**: 大小、分段数等

### 3. 网格系统
- **顶点数据管理**: 顶点和索引数据的存储和访问
- **网格信息**: 顶点数量、索引数量查询

### 4. 纹理系统
- **纯色纹理**: 创建指定颜色的纹理
- **棋盘纹理**: 创建棋盘格图案纹理
- **纹理参数**: 宽度、高度、格式配置

## 使用方式

### 方式一：直接运行测试程序

我们提供了三个测试程序来演示 AquaVisual 的功能：

#### 1. SimpleExample.exe
- **功能**: 基础功能测试
- **运行**: `.\build\bin\Debug\SimpleExample.exe`
- **内容**: 初始化、数学运算、几何体创建的简单演示

#### 2. TestAquaVisual.exe
- **功能**: 综合测试套件
- **运行**: `.\build\bin\Debug\TestAquaVisual.exe`
- **内容**: 
  - 库初始化测试
  - 数学库功能测试
  - 几何体生成测试
  - 网格操作测试
  - 纹理系统测试
  - 性能测试

#### 3. SceneDemo.exe
- **功能**: 高级场景演示
- **运行**: `.\build\bin\Debug\SceneDemo.exe`
- **内容**:
  - 复杂场景创建（地面、立方体、球体、三角形）
  - 动画模拟（旋转、移动）
  - 射线相交测试
  - 场景统计信息

### 方式二：使用 Python 脚本

我们提供了一个 Python 脚本来方便地运行和管理所有演示程序：

```bash
# 运行所有演示程序
python run_demos.py --all

# 交互式模式（可选择运行特定程序）
python run_demos.py --interactive

# 默认交互式模式
python run_demos.py
```

### 方式三：在 C++ 项目中使用

#### 基本使用步骤

1. **初始化库**:
```cpp
#include <AquaVisual/AquaVisual.h>

// 初始化
if (!AquaVisual::Initialize()) {
    // 处理初始化失败
}

// 使用库功能...

// 清理
AquaVisual::Shutdown();
```

2. **数学运算**:
```cpp
#include <AquaVisual/Math/Vector.h>
#include <AquaVisual/Math/Matrix.h>

// 向量运算
AquaVisual::Vector3 v1(1.0f, 2.0f, 3.0f);
AquaVisual::Vector3 v2(4.0f, 5.0f, 6.0f);
auto sum = v1 + v2;
float dot = v1.Dot(v2);
auto cross = v1.Cross(v2);

// 矩阵运算
auto identity = AquaVisual::Matrix4::Identity();
auto result = identity * identity;
```

3. **创建几何体**:
```cpp
#include <AquaVisual/Primitives.h>

// 创建基础几何体
auto triangle = AquaVisual::Primitives::CreateTriangle();
auto cube = AquaVisual::Primitives::CreateCube(2.0f);
auto sphere = AquaVisual::Primitives::CreateSphere(1.0f, 32);
auto plane = AquaVisual::Primitives::CreatePlane(10.0f, 10.0f);
```

4. **创建纹理**:
```cpp
#include <AquaVisual/Resources/Texture.h>

// 创建纯色纹理
auto redTexture = AquaVisual::Texture::CreateSolid(256, 256, 255, 0, 0);

// 创建棋盘纹理
auto checkerboard = AquaVisual::Texture::CreateCheckerboard(512, 512, 64);
```

5. **网格操作**:
```cpp
#include <AquaVisual/Resources/Mesh.h>

auto mesh = AquaVisual::Primitives::CreateCube();
if (mesh) {
    auto vertices = mesh->GetVertices();
    auto indices = mesh->GetIndices();
    uint32_t vertexCount = mesh->GetVertexCount();
    uint32_t indexCount = mesh->GetIndexCount();
}
```

## 构建要求

- **编译器**: Visual Studio 2019+ 或支持 C++17 的编译器
- **CMake**: 3.15+
- **依赖项**: 
  - GLFW (可选，用于示例程序)
  - GLM (可选，如未找到将使用内置数学库)

## 构建步骤

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake .. -DAQUAVISUAL_BUILD_EXAMPLES=ON

# 构建
cmake --build . --config Debug

# 运行测试
python ../run_demos.py --all
```

## 示例输出

### TestAquaVisual 输出示例:
```
=== AquaVisual Library Test Suite ===

1. Testing Initialization...
  [OK] AquaVisual initialized successfully

2. Testing Math Library...
  [OK] Vector addition: (5, 7, 9)
  [OK] Dot product: 32
  [OK] Cross product: (-3, 6, -3)

3. Testing Geometry Generation...
  [OK] Triangle created with 3 vertices
  [OK] Cube created with 24 vertices
  [OK] Sphere created with 289 vertices

4. Testing Texture System...
  [OK] Red texture created: 64x64
  [OK] Checkerboard texture created: 128x128

=== All Tests Completed ===
```

## 性能特点

- **轻量级**: 最小化的依赖和资源占用
- **高效**: 快速的几何体生成（100个立方体 < 1ms）
- **内存安全**: 使用智能指针管理资源
- **跨平台**: 支持 Windows、Linux、macOS

## 注意事项

1. **初始化**: 使用任何功能前必须调用 `AquaVisual::Initialize()`
2. **清理**: 程序结束前调用 `AquaVisual::Shutdown()`
3. **内存管理**: 所有创建的对象都使用智能指针，无需手动释放
4. **线程安全**: 当前版本不保证线程安全，请在单线程环境中使用

## 扩展开发

AquaVisual 采用模块化设计，可以轻松扩展：

- **添加新的几何体**: 在 `Primitives` 命名空间中添加新函数
- **扩展数学库**: 在 `Math` 命名空间中添加新的数学类型和函数
- **增强纹理系统**: 在 `Texture` 类中添加新的纹理生成方法
- **集成渲染器**: 通过 `Renderer` 接口集成实际的渲染后端

## 故障排除

### 常见问题

1. **构建失败**: 确保安装了正确版本的 CMake 和编译器
2. **找不到 GLFW**: 使用 vcpkg 或其他包管理器安装 GLFW
3. **示例程序无法运行**: 检查是否正确设置了 `AQUAVISUAL_BUILD_EXAMPLES=ON`

### 获取帮助

如果遇到问题，可以：
1. 查看构建日志中的错误信息
2. 运行 `python run_demos.py --all` 进行全面测试
3. 检查示例程序的源代码以了解正确的使用方法