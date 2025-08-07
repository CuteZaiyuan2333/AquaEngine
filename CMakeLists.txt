cmake_minimum_required(VERSION 3.20)

project(AquaEngine 
    VERSION 0.1.0
    DESCRIPTION "A modern 3D game engine with Vulkan renderer"
    LANGUAGES CXX
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 编译选项
if(MSVC)
    add_compile_options(/W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找Vulkan
find_package(Vulkan REQUIRED)

# 查找GLFW (用于窗口管理)
find_package(glfw3 REQUIRED)

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 添加子目录
add_subdirectory(Core)
add_subdirectory(Engine)
add_subdirectory(Examples)

# 创建主要的引擎库
add_library(AquaEngine STATIC)

# 设置目标属性
set_target_properties(AquaEngine PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# 链接库
target_link_libraries(AquaEngine 
    PUBLIC 
        Vulkan::Vulkan
        glfw
)

# 包含头文件目录
target_include_directories(AquaEngine 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Vulkan_INCLUDE_DIRS}
)

# 编译定义
target_compile_definitions(AquaEngine 
    PUBLIC
        $<$<CONFIG:Debug>:AQUA_DEBUG>
        $<$<CONFIG:Release>:AQUA_RELEASE>
)