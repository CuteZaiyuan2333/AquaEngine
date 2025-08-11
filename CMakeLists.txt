cmake_minimum_required(VERSION 3.16)

# 设置项目名称
set(PROJECT_NAME dual_cube_textured_demo)
project(${PROJECT_NAME})

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 AquaVisual 库
find_package(AquaVisual REQUIRED PATHS "${CMAKE_CURRENT_SOURCE_DIR}/AquaVisual/build")

# 查找 Vulkan
find_package(Vulkan REQUIRED)

# 创建可执行文件
add_executable(${PROJECT_NAME} dual_cube_textured_demo.cpp)

# 直接指定 GLFW 库路径
set(GLFW_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/GLFW/include")
set(GLFW_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/GLFW/lib-vc2022/glfw3.lib")

target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE
    AquaVisual::AquaVisual
    Vulkan::Vulkan
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/GLFW/lib-vc2022/glfw3.lib"
)

# 设置包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/AquaVisual/Include
    ${Vulkan_INCLUDE_DIRS}
)

# 编译选项
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        VK_USE_PLATFORM_WIN32_KHR
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
endif()

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 复制着色器文件到输出目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/AquaVisual/Shaders"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders"
    COMMENT "Copying shaders to output directory"
)

# 设置工作目录为输出目录（用于调试）
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

message(STATUS "MinecraftStyleDemo configured successfully")
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "Output directory: ${CMAKE_BINARY_DIR}/bin")