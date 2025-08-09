cmake_minimum_required(VERSION 3.20)

# AquaEngine 项目 - 现代化游戏引擎套件
project(AquaEngine 
    VERSION 1.0.0
    DESCRIPTION "Modern Game Engine Suite with AquaVisual Renderer and AquaCore Game Engine"
    LANGUAGES CXX
)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译器特定设置
if(MSVC)
    add_compile_options(/W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 构建类型设置
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

# 项目选项
option(AQUA_BUILD_VISUAL "Build AquaVisual renderer library" ON)
option(AQUA_BUILD_CORE "Build AquaCore game engine (requires AquaVisual)" OFF)
option(AQUA_BUILD_EXAMPLES "Build example applications" ON)
option(AQUA_BUILD_TESTS "Build unit tests" OFF)

# 全局包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 子项目构建
if(AQUA_BUILD_VISUAL)
    message(STATUS "Building AquaVisual renderer library...")
    add_subdirectory(AquaVisual)
endif()

if(AQUA_BUILD_CORE)
    if(NOT AQUA_BUILD_VISUAL)
        message(FATAL_ERROR "AquaCore requires AquaVisual to be built. Please enable AQUA_BUILD_VISUAL.")
    endif()
    message(STATUS "Building AquaCore game engine...")
    add_subdirectory(AquaCore)
endif()

# 添加窗口演示程序
if(AQUA_BUILD_VISUAL)
    # 查找Vulkan
    find_package(Vulkan REQUIRED)
    
    # 查找GLFW (尝试多种方式)
    find_package(glfw3 QUIET CONFIG)
    if(NOT glfw3_FOUND)
        find_package(glfw3 QUIET)
    endif()
    if(NOT glfw3_FOUND)
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(GLFW3 QUIET glfw3)
        endif()
    endif()
    
    # 手动查找GLFW (如果上述方法都失败)
    if(NOT glfw3_FOUND AND NOT GLFW3_FOUND)
        # 常见的GLFW安装路径，包括项目本地路径
        set(GLFW_SEARCH_PATHS
            "${CMAKE_SOURCE_DIR}/ThirdParty/GLFW"
            "C:/Program Files/GLFW"
            "C:/Program Files (x86)/GLFW"
            "C:/glfw"
            "C:/vcpkg/installed/x64-windows"
            "$ENV{GLFW_ROOT}"
        )
        
        find_path(GLFW_INCLUDE_DIR GLFW/glfw3.h
            PATHS ${GLFW_SEARCH_PATHS}
            PATH_SUFFIXES include
        )
        
        find_library(GLFW_LIBRARY
            NAMES glfw3 glfw3_mt glfw
            PATHS ${GLFW_SEARCH_PATHS}
            PATH_SUFFIXES lib lib64 lib-vc2022 lib-vc2019 lib-vc2017 lib-vc2015
        )
        
        if(GLFW_INCLUDE_DIR AND GLFW_LIBRARY)
            set(GLFW_MANUAL_FOUND TRUE)
            message(STATUS "Found GLFW manually: ${GLFW_LIBRARY}")
        endif()
    endif()
    
    # 添加窗口演示可执行文件
    add_executable(CubeWindowDemo cube_window_demo.cpp)
    
    # 设置头文件目录
    target_include_directories(CubeWindowDemo PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/AquaVisual/Include
        ${Vulkan_INCLUDE_DIRS}
    )
    
    # 链接库
    target_link_libraries(CubeWindowDemo PRIVATE
        AquaVisual
        ${Vulkan_LIBRARIES}
    )
    
    # 如果找到GLFW，链接它
    if(glfw3_FOUND)
        target_link_libraries(CubeWindowDemo PRIVATE glfw)
        target_compile_definitions(CubeWindowDemo PRIVATE AQUA_HAS_GLFW=1)
        message(STATUS "GLFW found and linked successfully")
    elseif(GLFW3_FOUND)
        target_link_libraries(CubeWindowDemo PRIVATE ${GLFW3_LIBRARIES})
        target_include_directories(CubeWindowDemo PRIVATE ${GLFW3_INCLUDE_DIRS})
        target_compile_definitions(CubeWindowDemo PRIVATE AQUA_HAS_GLFW=1)
        message(STATUS "GLFW found via pkg-config and linked successfully")
    elseif(GLFW_MANUAL_FOUND)
        target_link_libraries(CubeWindowDemo PRIVATE ${GLFW_LIBRARY})
        target_include_directories(CubeWindowDemo PRIVATE ${GLFW_INCLUDE_DIR})
        target_compile_definitions(CubeWindowDemo PRIVATE AQUA_HAS_GLFW=1)
        message(STATUS "GLFW found manually and linked successfully")
    else()
        message(WARNING "GLFW not found - window management will be limited")
    endif()
    
    # 设置编译定义
    target_compile_definitions(CubeWindowDemo PRIVATE
        AQUA_HAS_VULKAN=1
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
    
    # 设置运行时库
    if(MSVC)
        set_property(TARGET CubeWindowDemo PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    endif()
    
    # 复制着色器文件
    set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/AquaVisual/Shaders")
    set(SHADER_DEST_DIR_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/Debug/Shaders")
    set(SHADER_DEST_DIR_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/Release/Shaders")
    
    # 创建着色器目录
    file(MAKE_DIRECTORY ${SHADER_DEST_DIR_DEBUG})
    file(MAKE_DIRECTORY ${SHADER_DEST_DIR_RELEASE})
    
    # 复制所有.spv文件
    file(GLOB SHADER_FILES "${SHADER_SOURCE_DIR}/*.spv")
    foreach(SHADER_FILE ${SHADER_FILES})
        get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)
        configure_file(${SHADER_FILE} ${SHADER_DEST_DIR_DEBUG}/${SHADER_NAME} COPYONLY)
        configure_file(${SHADER_FILE} ${SHADER_DEST_DIR_RELEASE}/${SHADER_NAME} COPYONLY)
    endforeach()
    
    message(STATUS "CubeWindowDemo 已添加到构建目标")
endif()

# 项目信息输出
message(STATUS "")
message(STATUS "=== AquaEngine Configuration ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "AquaVisual: ${AQUA_BUILD_VISUAL}")
message(STATUS "AquaCore: ${AQUA_BUILD_CORE}")
message(STATUS "Examples: ${AQUA_BUILD_EXAMPLES}")
message(STATUS "Tests: ${AQUA_BUILD_TESTS}")
message(STATUS "================================")
message(STATUS "")