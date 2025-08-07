# AquaEngine 依赖说明

## 必需依赖

### 1. Vulkan SDK
- **下载地址**: https://vulkan.lunarg.com/
- **版本要求**: 1.3.0 或更高
- **安装说明**: 
  - 下载适合你操作系统的SDK
  - 安装时确保添加到系统PATH
  - 验证安装：在命令行运行 `vulkaninfo`

### 2. GLFW
- **用途**: 窗口管理和输入处理
- **安装方式**: 
  - **Windows**: 使用vcpkg: `vcpkg install glfw3:x64-windows`
  - **Linux**: `sudo apt-get install libglfw3-dev`
  - **macOS**: `brew install glfw`

### 3. CMake
- **版本要求**: 3.20 或更高
- **下载地址**: https://cmake.org/download/

### 4. 编译器
- **Windows**: Visual Studio 2022 或更高版本
- **Linux**: GCC 10+ 或 Clang 12+
- **macOS**: Xcode 13+ 或 Clang 12+

## 可选依赖

### 1. Python (用于脚本支持)
- **版本要求**: Python 3.8+
- **用途**: 游戏脚本和工具开发

### 2. Git
- **用途**: 版本控制
- **推荐**: 用于参与项目开发

## 安装验证

### Windows (使用vcpkg)
```batch
# 安装vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# 安装依赖
.\vcpkg install glfw3:x64-windows
.\vcpkg integrate install
```

### 验证Vulkan安装
```bash
# 检查Vulkan
vulkaninfo

# 检查环境变量
echo $VULKAN_SDK  # Linux/macOS
echo %VULKAN_SDK% # Windows
```

## 构建说明

1. 确保所有依赖已正确安装
2. 克隆项目：`git clone <repository-url>`
3. 运行构建脚本：
   - Windows: `build.bat`
   - Linux/macOS: `./build.sh` (待创建)

## 常见问题

### Q: CMake找不到Vulkan
**A**: 确保VULKAN_SDK环境变量已设置，并且Vulkan SDK已正确安装

### Q: 找不到GLFW
**A**: 
- 使用vcpkg安装GLFW
- 或者手动设置GLFW_ROOT环境变量

### Q: 编译错误：C++20特性不支持
**A**: 确保使用支持C++20的编译器版本

## 开发环境推荐

- **IDE**: Visual Studio 2022, CLion, 或 VS Code
- **调试工具**: RenderDoc (图形调试)
- **性能分析**: Intel VTune, AMD uProf