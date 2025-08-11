cmake_minimum_required(VERSION 3.16)
project(TexturedCubeDemo)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Vulkan SDK
find_package(Vulkan REQUIRED)

# 添加包含目录
include_directories(${CMAKE_SOURCE_DIR}/../AquaVisual/Include)
include_directories(${CMAKE_SOURCE_DIR}/../ThirdParty/STB)
include_directories(${CMAKE_SOURCE_DIR}/../ThirdParty/GLFW/include)
include_directories(${Vulkan_INCLUDE_DIRS})

# 添加预处理器定义
add_definitions(-DSTB_IMAGE_IMPLEMENTATION)
add_definitions(-DAQUA_HAS_VULKAN)
add_definitions(-DAQUA_HAS_GLFW)

# Windows 平台特定设置
if(WIN32)
    add_definitions(-DNOMINMAX)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# 创建可执行文件
add_executable(TexturedCubeDemo 
    ${CMAKE_SOURCE_DIR}/../textured_cube_demo.cpp
    ${CMAKE_SOURCE_DIR}/../ThirdParty/STB/stb_image_impl.cpp
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Source/AquaVisual.cpp
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Source/AquaVisualMVP.cpp
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Source/Resources/Texture.cpp
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Source/Resources/Mesh.cpp
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Source/Resources/Primitives.cpp
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Source/Core/VulkanRenderer.cpp
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Source/Core/VulkanRendererImpl.cpp
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Source/Core/BufferManager.cpp
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Source/Core/ShaderManager.cpp
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Source/Core/RenderPipeline.cpp
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Source/Core/Renderer.cpp
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Source/Core/Camera.cpp
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Source/Core/Window.cpp
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Source/Math/Matrix.cpp
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Source/Math/Vector.cpp
)

# 链接库
target_link_libraries(TexturedCubeDemo 
    ${Vulkan_LIBRARIES}
    ${CMAKE_SOURCE_DIR}/../ThirdParty/GLFW/lib-vc2022/glfw3.lib
)

# 设置调试工作目录
set_target_properties(TexturedCubeDemo PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/.."
)