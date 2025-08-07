# Core模块CMakeLists.txt

# 收集所有源文件
file(GLOB_RECURSE CORE_SOURCES
    "Math/*.cpp"
    "Math/*.h"
    "Renderer/*.cpp"
    "Renderer/*.h"
    "Platform/*.cpp"
    "Platform/*.h"
    "Memory/*.cpp"
    "Memory/*.h"
)

# 创建Core库
add_library(AquaCore STATIC ${CORE_SOURCES})

# 设置目标属性
set_target_properties(AquaCore PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# 设置GLFW路径
set(GLFW_ROOT "D:/WinApplication/glfw/glfw-3.4.bin.WIN64")
set(GLFW_INCLUDE_DIR "${GLFW_ROOT}/include")
set(GLFW_LIBRARY_DIR "${GLFW_ROOT}/lib-vc2022")

# 链接依赖
target_link_libraries(AquaCore 
    PUBLIC 
        Vulkan::Vulkan
        "${GLFW_LIBRARY_DIR}/glfw3.lib"
)

# 包含目录
target_include_directories(AquaCore 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${Vulkan_INCLUDE_DIRS}
        ${GLFW_INCLUDE_DIR}
)

# 编译定义
target_compile_definitions(AquaCore 
    PUBLIC
        $<$<CONFIG:Debug>:AQUA_DEBUG>
        $<$<CONFIG:Release>:AQUA_RELEASE>
)