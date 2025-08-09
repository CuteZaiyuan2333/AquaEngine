cmake_minimum_required(VERSION 3.16)
project(RotatingCubeDemo)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 查找 Vulkan
find_package(Vulkan REQUIRED)

# 设置 AquaVisual 路径
set(AQUAVISUAL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../AquaVisual")
set(AQUAVISUAL_INCLUDE_DIR "${AQUAVISUAL_ROOT}/Include")
set(AQUAVISUAL_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../build_window_demo/AquaVisual/Debug")

# 查找 AquaVisual 库
find_library(AQUAVISUAL_LIBRARY
    NAMES AquaVisual
    PATHS ${AQUAVISUAL_BINARY_DIR}
    NO_DEFAULT_PATH
)

if(NOT AQUAVISUAL_LIBRARY)
    message(FATAL_ERROR "AquaVisual library not found! Please build AquaVisual first.")
endif()

# 设置 GLFW 路径
set(GLFW_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParty/GLFW")
set(GLFW_INCLUDE_DIR "${GLFW_ROOT}/include")
set(GLFW_LIBRARY_DIR "${GLFW_ROOT}/lib-vc2022")

# 查找 GLFW 库
find_library(GLFW_LIBRARY
    NAMES glfw3
    PATHS ${GLFW_LIBRARY_DIR}
    NO_DEFAULT_PATH
)

if(NOT GLFW_LIBRARY)
    message(FATAL_ERROR "GLFW library not found!")
endif()

# 创建可执行文件
add_executable(RotatingCubeDemo
    rotating_cube_demo.cpp
)

# 设置包含目录
target_include_directories(RotatingCubeDemo PRIVATE
    ${AQUAVISUAL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${Vulkan_INCLUDE_DIRS}
)

# 链接库
target_link_libraries(RotatingCubeDemo
    ${AQUAVISUAL_LIBRARY}
    ${GLFW_LIBRARY}
    ${Vulkan_LIBRARIES}
)

# Windows 特定设置
if(WIN32)
    target_link_libraries(RotatingCubeDemo
        user32
        gdi32
        shell32
    )
    
    # 设置工作目录
    set_target_properties(RotatingCubeDemo PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()

# 复制着色器文件
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../AquaVisual/Shaders")
set(SHADER_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/Shaders")
set(SHADER_DEBUG_DIR "${CMAKE_CURRENT_BINARY_DIR}/Debug/Shaders")

# 创建着色器目录
file(MAKE_DIRECTORY ${SHADER_DEST_DIR})
file(MAKE_DIRECTORY ${SHADER_DEBUG_DIR})

# 复制编译好的着色器到两个位置
file(GLOB SHADER_FILES "${SHADER_SOURCE_DIR}/*.spv")
foreach(SHADER_FILE ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)
    configure_file(${SHADER_FILE} ${SHADER_DEST_DIR}/${SHADER_NAME} COPYONLY)
    configure_file(${SHADER_FILE} ${SHADER_DEBUG_DIR}/${SHADER_NAME} COPYONLY)
endforeach()

# 添加自定义目标来在构建时复制着色器
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_SOURCE_DIR} ${SHADER_DEST_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_SOURCE_DIR} ${SHADER_DEBUG_DIR}
    COMMENT "Copying shader files"
)
add_dependencies(RotatingCubeDemo copy_shaders)

# 输出信息
message(STATUS "RotatingCubeDemo Configuration:")
message(STATUS "  AquaVisual Include: ${AQUAVISUAL_INCLUDE_DIR}")
message(STATUS "  AquaVisual Library: ${AQUAVISUAL_LIBRARY}")
message(STATUS "  GLFW Include: ${GLFW_INCLUDE_DIR}")
message(STATUS "  GLFW Library: ${GLFW_LIBRARY}")
message(STATUS "  Vulkan Include: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "  Vulkan Libraries: ${Vulkan_LIBRARIES}")
message(STATUS "  Shader Directory: ${SHADER_DEST_DIR}")