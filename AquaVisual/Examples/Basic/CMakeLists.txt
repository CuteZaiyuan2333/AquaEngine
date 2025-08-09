# Simple Example
add_executable(SimpleExample SimpleExample.cpp)

target_link_libraries(SimpleExample 
    PRIVATE 
        AquaVisual
)

target_include_directories(SimpleExample 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Include
)

# Set output directory
set_target_properties(SimpleExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Comprehensive Test
add_executable(ComprehensiveTest ComprehensiveTest.cpp)

target_link_libraries(ComprehensiveTest 
    PRIVATE 
        AquaVisual
)

target_include_directories(ComprehensiveTest 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Include
)

# Set output directory
set_target_properties(ComprehensiveTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Test AquaVisual
add_executable(TestAquaVisual TestAquaVisual.cpp)

target_link_libraries(TestAquaVisual 
    PRIVATE 
        AquaVisual
)

target_include_directories(TestAquaVisual 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Include
)

# Set output directory
set_target_properties(TestAquaVisual PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Scene Demo
add_executable(SceneDemo SceneDemo.cpp)

target_link_libraries(SceneDemo 
    PRIVATE 
        AquaVisual
)

target_include_directories(SceneDemo 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Include
)

# Set output directory
set_target_properties(SceneDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# VulkanDemo 程序
add_executable(VulkanDemo VulkanDemo.cpp)

target_link_libraries(VulkanDemo PRIVATE
    AquaVisual
    ${Vulkan_LIBRARIES}
)

if(glfw3_FOUND)
    target_link_libraries(VulkanDemo PRIVATE glfw)
endif()

target_include_directories(VulkanDemo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Include
    ${Vulkan_INCLUDE_DIRS}
)

target_compile_features(VulkanDemo PRIVATE cxx_std_17)

# 设置输出目录
set_target_properties(VulkanDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# SimpleVulkanTest 程序
add_executable(SimpleVulkanTest SimpleVulkanTest.cpp)

target_link_libraries(SimpleVulkanTest PRIVATE
    AquaVisual
)

target_include_directories(SimpleVulkanTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Include
)

target_compile_features(SimpleVulkanTest PRIVATE cxx_std_17)

# 设置输出目录
set_target_properties(SimpleVulkanTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# VulkanWindowDemo 程序
add_executable(VulkanWindowDemo VulkanWindowDemo.cpp)

target_link_libraries(VulkanWindowDemo PRIVATE
    AquaVisual
)

target_include_directories(VulkanWindowDemo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Include
)

target_compile_features(VulkanWindowDemo PRIVATE cxx_std_17)

# 设置输出目录
set_target_properties(VulkanWindowDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)