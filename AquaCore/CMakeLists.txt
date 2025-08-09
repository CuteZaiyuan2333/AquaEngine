cmake_minimum_required(VERSION 3.20)

# AquaCore - 基于 AquaVisual 的游戏引擎
project(AquaCore 
    VERSION 1.0.0
    DESCRIPTION "Game Engine built on top of AquaVisual Renderer"
    LANGUAGES CXX
)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找依赖
find_package(AquaVisual REQUIRED)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${AquaVisual_INCLUDE_DIRS}
)

# 源文件（当前为空，未来添加）
set(AQUACORE_SOURCES
    # Source/Core/Engine.cpp
    # Source/Scene/SceneManager.cpp
    # Source/ECS/EntityManager.cpp
    # Source/Physics/PhysicsWorld.cpp
    # Source/Audio/AudioSystem.cpp
    # Source/Input/InputManager.cpp
    # Source/Resources/ResourceManager.cpp
)

# 头文件（当前为空，未来添加）
set(AQUACORE_HEADERS
    # Include/AquaCore/Core/Engine.h
    # Include/AquaCore/Scene/SceneManager.h
    # Include/AquaCore/ECS/EntityManager.h
    # Include/AquaCore/Physics/PhysicsWorld.h
    # Include/AquaCore/Audio/AudioSystem.h
    # Include/AquaCore/Input/InputManager.h
    # Include/AquaCore/Resources/ResourceManager.h
)

# 创建库（当前为空，未来实现）
if(AQUACORE_SOURCES)
    add_library(AquaCore ${AQUACORE_SOURCES} ${AQUACORE_HEADERS})
    
    # 链接 AquaVisual
    target_link_libraries(AquaCore 
        PUBLIC AquaVisual
    )
    
    # 设置目标属性
    set_target_properties(AquaCore PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
        PUBLIC_HEADER "${AQUACORE_HEADERS}"
    )
    
    # 导出目标
    install(TARGETS AquaCore
        EXPORT AquaCoreTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION include/AquaCore
    )
else()
    message(STATUS "AquaCore: No source files yet - placeholder project created")
endif()

# 示例程序（未来添加）
if(AQUA_BUILD_EXAMPLES AND AQUACORE_SOURCES)
    add_subdirectory(Examples)
endif()

message(STATUS "AquaCore: Placeholder project configured for future development")