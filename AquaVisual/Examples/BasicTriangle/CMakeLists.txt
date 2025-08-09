cmake_minimum_required(VERSION 3.16)

project(BasicTriangle)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找依赖
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm QUIET)

# 添加可执行文件
add_executable(BasicTriangle main.cpp)

# 链接AquaVisual库
target_link_libraries(BasicTriangle 
    PRIVATE 
    AquaVisual
    Vulkan::Vulkan
    glfw
)

# 如果找到GLM，则链接它
if(glm_FOUND)
    target_link_libraries(BasicTriangle PRIVATE glm::glm)
endif()

# 设置包含目录
target_include_directories(BasicTriangle 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Include
)

# 复制着色器文件到输出目录
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../Shaders 
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# 设置工作目录
set_target_properties(BasicTriangle PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)