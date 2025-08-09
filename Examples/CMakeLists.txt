cmake_minimum_required(VERSION 3.16)

# MVP Demo
add_executable(MVP_Demo MVP_Demo.cpp)

# 链接AquaVisual库
target_link_libraries(MVP_Demo PRIVATE AquaVisual)

# 设置输出目录
set_target_properties(MVP_Demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 如果是Windows，复制必要的DLL
if(WIN32)
    # 这里可以添加复制Vulkan DLL等的逻辑
endif()