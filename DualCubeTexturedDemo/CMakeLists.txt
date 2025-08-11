cmake_minimum_required(VERSION 3.20)
project(DualCubeTexturedDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加AquaVisual库
add_subdirectory(../AquaVisual AquaVisual)

# 创建可执行文件
add_executable(DualCubeTexturedDemo ../dual_cube_textured_demo.cpp)

# 链接AquaVisual库
target_link_libraries(DualCubeTexturedDemo PRIVATE AquaVisual)

# 设置包含目录
target_include_directories(DualCubeTexturedDemo PRIVATE 
    ../AquaVisual/Include
)

# 复制资源文件
add_custom_command(TARGET DualCubeTexturedDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Shaders
    ${CMAKE_BINARY_DIR}/AquaVisual/Shaders
)

add_custom_command(TARGET DualCubeTexturedDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/../AquaVisual/Images
    ${CMAKE_BINARY_DIR}/AquaVisual/Images
)