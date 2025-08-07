@echo off
echo ========================================
echo AquaEngine 构建脚本
echo ========================================

REM 检查是否存在Build目录
if not exist "Build" (
    echo 创建Build目录...
    mkdir Build
)

cd Build

echo 配置CMake项目...
cmake .. -G "Visual Studio 17 2022" -A x64

if %ERRORLEVEL% neq 0 (
    echo CMake配置失败！
    echo 请确保已安装：
    echo - Visual Studio 2022 或更高版本
    echo - Vulkan SDK
    echo - CMake 3.20+
    pause
    exit /b 1
)

echo 开始编译...
cmake --build . --config Debug

if %ERRORLEVEL% neq 0 (
    echo 编译失败！
    pause
    exit /b 1
)

echo ========================================
echo 编译成功！
echo 可执行文件位置: Build\bin\examples\Debug\HelloTriangle.exe
echo ========================================

pause