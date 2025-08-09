@echo off
echo ========================================
echo Building Rotating Cube Demo
echo ========================================

:: 检查 AquaVisual 是否已构建
if not exist "..\..\build_window_demo\AquaVisual\Debug\AquaVisual.lib" (
    echo Error: AquaVisual library not found!
    echo Please build AquaVisual first by running:
    echo   cd ..\..\
    echo   cmake -B build_window_demo -DCMAKE_BUILD_TYPE=Debug
    echo   cmake --build build_window_demo --config Debug
    pause
    exit /b 1
)

:: 创建构建目录
if not exist "build" mkdir build

:: 配置项目
echo Configuring project...
cmake -B build -DCMAKE_BUILD_TYPE=Debug
if %ERRORLEVEL% neq 0 (
    echo Configuration failed!
    pause
    exit /b 1
)

:: 构建项目
echo Building project...
cmake --build build --config Debug
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo ========================================
echo Build completed successfully!
echo ========================================

:: 运行程序
echo Running Rotating Cube Demo...
echo Press any key to start the demo...
pause

cd build\Debug
RotatingCubeDemo.exe
cd ..\..

echo ========================================
echo Demo completed!
echo ========================================
pause