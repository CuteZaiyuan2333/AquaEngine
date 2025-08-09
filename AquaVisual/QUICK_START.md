# AquaVisual MVP 快速开始指南

## 前置要求

### 系统要求
- Windows 10/11 (64位)
- 支持 Vulkan 1.0+ 的显卡
- Visual Studio 2019 或更新版本

### 依赖库
- **Vulkan SDK** (1.3.0+)
- **GLFW** (3.3+)
- **GLM** (0.9.9+)
- **CMake** (3.16+)

## 安装依赖

### 1. 安装 Vulkan SDK
1. 从 [LunarG](https://vulkan.lunarg.com/) 下载最新的 Vulkan SDK
2. 运行安装程序，确保添加到系统 PATH
3. 验证安装：打开命令行运行 `vulkaninfo`

### 2. 安装 GLFW
```bash
# 使用 vcpkg (推荐)
vcpkg install glfw3:x64-windows

# 或者手动下载编译
# https://www.glfw.org/download.html
```

### 3. 安装 GLM
```bash
# 使用 vcpkg
vcpkg install glm:x64-windows

# 或者直接下载头文件库
# https://github.com/g-truc/glm
```

## 构建项目

### 1. 克隆或下载项目
```bash
# 如果使用 Git
git clone <repository-url>
cd AquaVisual

# 或者直接下载解压到目录
```

### 2. 生成项目文件
```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
```

### 3. 编译项目
```bash
# 使用 CMake 编译
cmake --build . --config Release

# 或者在 Visual Studio 中打开 .sln 文件编译
```

## 第一个程序

### 1. 基础三角形示例

创建一个新的 C++ 文件 `my_first_triangle.cpp`：

```cpp
#include <AquaVisual/AquaVisualMVP.h>
#include <iostream>

using namespace AquaVisual;

int main() {
    std::cout << "AquaVisual MVP Version: " << GetMVPVersion() << std::endl;

    try {
        // 检查 Vulkan 支持
        if (!IsVulkanAvailable()) {
            std::cerr << "Vulkan is not available!" << std::endl;
            return -1;
        }

        // 创建渲染器
        auto renderer = QuickStart::CreateDefaultRenderer(800, 600, "我的第一个三角形");
        
        // 初始化渲染器
        AQUAVISUAL_CHECK(renderer->Initialize(), "Failed to initialize renderer");

        // 创建相机
        auto camera = QuickStart::CreateDefaultCamera(800.0f / 600.0f);

        // 创建三角形网格
        auto triangle = Mesh::CreateTriangle(1.0f);

        // 创建纹理
        auto texture = Texture::CreateCheckerboard(256, 256);

        AQUAVISUAL_LOG("进入渲染循环...");

        // 渲染循环
        while (!renderer->ShouldClose()) {
            renderer->PollEvents();

            if (renderer->BeginFrame()) {
                // 清空屏幕 (深蓝色背景)
                renderer->Clear(0.1f, 0.2f, 0.4f, 1.0f);

                // 设置相机
                renderer->SetCamera(camera);

                // 渲染三角形
                renderer->RenderMesh(*triangle, texture.get());

                renderer->EndFrame();
            }
        }

        // 清理
        renderer->Shutdown();
        std::cout << "程序成功结束!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
```

### 2. 编译和运行

创建 `CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyFirstTriangle)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(MyFirstTriangle my_first_triangle.cpp)

target_link_libraries(MyFirstTriangle 
    PRIVATE 
    AquaVisual
    Vulkan::Vulkan
    glfw
    glm::glm
)

target_include_directories(MyFirstTriangle 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../Include
)
```

编译运行：

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
./MyFirstTriangle
```

## 更多示例

### 1. 渲染立方体

```cpp
// 创建立方体而不是三角形
auto cube = Mesh::CreateCube(1.0f);

// 在渲染循环中
renderer->RenderMesh(*cube, texture.get());
```

### 2. 相机控制

```cpp
// 设置相机位置
camera.SetPosition(Vec3(0.0f, 0.0f, 5.0f));
camera.SetTarget(Vec3(0.0f, 0.0f, 0.0f));

// 更新投影矩阵
camera.SetPerspective(45.0f * 3.14159f / 180.0f, 800.0f / 600.0f, 0.1f, 100.0f);
```

### 3. 自定义纹理

```cpp
// 创建纯色纹理
auto redTexture = Texture::CreateSolid(256, 256, 255, 0, 0, 255);

// 从文件加载纹理 (如果支持)
auto fileTexture = Texture::CreateFromFile("path/to/texture.png");
```

## 常见问题

### Q: 编译时找不到 Vulkan
**A**: 确保已安装 Vulkan SDK 并设置了环境变量 `VULKAN_SDK`

### Q: 运行时提示 Vulkan 不可用
**A**: 检查显卡驱动是否支持 Vulkan，更新到最新驱动

### Q: 窗口创建失败
**A**: 确保 GLFW 正确安装，检查是否有其他程序占用显卡

### Q: 着色器编译失败
**A**: 确保着色器文件在正确的目录下，检查 SPIR-V 编译器是否可用

## 下一步

1. 查看 `Examples/BasicTriangle/` 目录中的完整示例
2. 阅读 `API_EXAMPLES.md` 了解更多 API 用法
3. 查看 `DEVELOPMENT_STATUS.md` 了解当前开发进度
4. 参考 `ARCHITECTURE_DESIGN.md` 了解架构设计

## 获取帮助

- 查看项目文档：`README.md`
- 技术问题：查看 `TECHNICAL_DECISIONS.md`
- 开发进度：查看 `DEVELOPMENT_STATUS.md`
- 集成指南：查看 `INTEGRATION_GUIDE.md`

---

**注意**: 这是 MVP 版本，功能有限。完整功能将在后续版本中逐步添加。