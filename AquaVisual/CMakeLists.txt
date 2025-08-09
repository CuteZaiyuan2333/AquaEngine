cmake_minimum_required(VERSION 3.16)
project(AquaVisual VERSION 0.1.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找依赖
find_package(Vulkan REQUIRED)

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)

# 源文件
set(AQUA_VISUAL_SOURCES
    # Main API
    Source/AquaVisual.cpp
    Source/AquaVisualMVP.cpp
    
    # Core
    Source/Core/Camera.cpp
    Source/Core/Material.cpp
    Source/Core/Window.cpp
    Source/Core/Renderer.cpp
    Source/Core/VulkanRenderer.cpp
    Source/Core/VulkanRendererImpl.cpp
    Source/Core/Texture.cpp
    Source/Core/ShaderManager.cpp
    Source/Core/RenderPipeline.cpp
    Source/Core/BufferManager.cpp
    
    # Resources
    Source/Resources/Mesh.cpp
    Source/Resources/Texture.cpp
    Source/Resources/Primitives.cpp
    
    # Math
    Source/Math/Vector.cpp
    Source/Math/Matrix.cpp
)

# 头文件
set(AQUA_VISUAL_HEADERS
    Include/AquaVisual/AquaVisual.h
    Include/AquaVisual/AquaVisualMVP.h
    Include/AquaVisual/Math.h
    Include/AquaVisual/Primitives.h
    Include/AquaVisual/Core/RenderAPI.h
    Include/AquaVisual/Core/VulkanRenderer.h
    Include/AquaVisual/Core/VulkanRendererImpl.h
    Include/AquaVisual/Core/Material.h
    Include/AquaVisual/Core/Camera.h
    Include/AquaVisual/Core/Renderer.h
    Include/AquaVisual/Core/Window.h
    Include/AquaVisual/Math/Matrix.h
    Include/AquaVisual/Math/Vector.h
    Include/AquaVisual/Resources/Mesh.h
    Include/AquaVisual/Resources/Texture.h
)

# 创建静态库
add_library(AquaVisual STATIC ${AQUA_VISUAL_SOURCES})

# 链接库
target_link_libraries(AquaVisual 
    PUBLIC 
        Vulkan::Vulkan
)

# 包含目录
target_include_directories(AquaVisual 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<INSTALL_INTERFACE:Include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

# 编译定义
target_compile_definitions(AquaVisual 
    PUBLIC
        AQUA_VISUAL_VERSION_MAJOR=0
        AQUA_VISUAL_VERSION_MINOR=1
        AQUA_VISUAL_VERSION_PATCH=0
    PRIVATE
        $<$<CONFIG:Debug>:AQUA_DEBUG>
        $<$<CONFIG:Debug>:VULKAN_VALIDATION_ENABLED>
)

# 检查是否有 GLFW 和 GLM
message(STATUS "Checking for optional dependencies...")

# 查找 GLFW (尝试多种方式)
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

if(glfw3_FOUND)
    target_link_libraries(AquaVisual PUBLIC glfw)
    target_compile_definitions(AquaVisual PRIVATE AQUA_HAS_GLFW)
    message(STATUS "Found GLFW: ${glfw3_DIR}")
elseif(GLFW3_FOUND)
    target_link_libraries(AquaVisual PUBLIC ${GLFW3_LIBRARIES})
    target_include_directories(AquaVisual PUBLIC ${GLFW3_INCLUDE_DIRS})
    target_compile_definitions(AquaVisual PRIVATE AQUA_HAS_GLFW)
    message(STATUS "Found GLFW via pkg-config")
elseif(GLFW_MANUAL_FOUND)
    target_link_libraries(AquaVisual PUBLIC ${GLFW_LIBRARY})
    target_include_directories(AquaVisual PRIVATE ${GLFW_INCLUDE_DIR})
    target_compile_definitions(AquaVisual PRIVATE AQUA_HAS_GLFW)
    message(STATUS "Found GLFW manually and linked successfully")
else()
    message(STATUS "GLFW not found - window management will be limited")
endif()

# 查找 GLM
find_package(glm QUIET)
if(glm_FOUND)
    target_link_libraries(AquaVisual PUBLIC glm::glm)
    target_compile_definitions(AquaVisual PRIVATE AQUA_HAS_GLM)
    message(STATUS "Found GLM: ${glm_DIR}")
else()
    message(STATUS "GLM not found - using built-in math library")
endif()

# 示例程序（仅在有依赖时构建）
option(AQUAVISUAL_BUILD_EXAMPLES "Build AquaVisual examples" ON)

if(AQUAVISUAL_BUILD_EXAMPLES AND glfw3_FOUND)
    add_subdirectory(Examples)
else()
    message(STATUS "Examples disabled - missing dependencies or option disabled")
endif()

# 安装配置
install(TARGETS AquaVisual
    EXPORT AquaVisualTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY Include/AquaVisual
    DESTINATION Include
)

# 导出配置
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/AquaVisualConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

export(EXPORT AquaVisualTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/AquaVisualTargets.cmake"
    NAMESPACE AquaVisual::
)

# 创建简单的配置文件
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/AquaVisualConfig.cmake"
"include(CMakeFindDependencyMacro)
find_dependency(Vulkan REQUIRED)
include(\"\${CMAKE_CURRENT_LIST_DIR}/AquaVisualTargets.cmake\")
")

install(EXPORT AquaVisualTargets
    FILE AquaVisualTargets.cmake
    NAMESPACE AquaVisual::
    DESTINATION lib/cmake/AquaVisual
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/AquaVisualConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/AquaVisualConfigVersion.cmake"
    DESTINATION lib/cmake/AquaVisual
)