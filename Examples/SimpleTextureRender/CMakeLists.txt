cmake_minimum_required(VERSION 3.20)

project(SimpleTextureRender)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(SimpleTextureRender main.cpp)

# Link libraries
target_link_libraries(SimpleTextureRender 
    AquaCore
    AquaEngine
)

# Include directories
target_include_directories(SimpleTextureRender PRIVATE
    ${CMAKE_SOURCE_DIR}/Core
    ${CMAKE_SOURCE_DIR}/Engine
)

# Set output directory
set_target_properties(SimpleTextureRender PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples
)