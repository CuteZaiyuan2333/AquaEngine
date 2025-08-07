cmake_minimum_required(VERSION 3.16)
project(SimpleTriangle)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Vulkan
find_package(Vulkan REQUIRED)

# 创建可执行文件
add_executable(SimpleTriangle simple_triangle.cpp)

# 链接Vulkan库
target_link_libraries(SimpleTriangle ${Vulkan_LIBRARIES})
target_include_directories(SimpleTriangle PRIVATE ${Vulkan_INCLUDE_DIRS})

# 设置输出目录
set_target_properties(SimpleTriangle PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)