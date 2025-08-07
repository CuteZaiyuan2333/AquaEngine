# Examples模块CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# HelloTriangle示例
add_executable(HelloTriangle
    HelloTriangle/main.cpp
)

# 设置C++标准
set_property(TARGET HelloTriangle PROPERTY CXX_STANDARD 20)
set_property(TARGET HelloTriangle PROPERTY CXX_STANDARD_REQUIRED ON)

# 链接库
target_link_libraries(HelloTriangle 
    AquaCore 
    AquaEngine
)

# 设置包含目录
target_include_directories(HelloTriangle PRIVATE
    ${CMAKE_SOURCE_DIR}
)

# 设置输出目录
set_target_properties(HelloTriangle PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples
)