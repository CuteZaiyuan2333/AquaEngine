# Engine模块CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# 收集所有源文件
file(GLOB_RECURSE ENGINE_SOURCES
    "Scene/*.cpp"
    "Scene/*.h"
    "Components/*.cpp"
    "Components/*.h"
    "Systems/*.cpp"
    "Systems/*.h"
)

# 创建Engine静态库
add_library(AquaEngine STATIC ${ENGINE_SOURCES})

# 设置C++标准
set_property(TARGET AquaEngine PROPERTY CXX_STANDARD 20)
set_property(TARGET AquaEngine PROPERTY CXX_STANDARD_REQUIRED ON)

# 链接Core库
target_link_libraries(AquaEngine PUBLIC AquaCore)

# 设置包含目录
target_include_directories(AquaEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)