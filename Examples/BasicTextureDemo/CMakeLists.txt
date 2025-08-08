cmake_minimum_required(VERSION 3.20)

# Create executable
add_executable(BasicTextureDemo main.cpp)

# Set C++ standard
set_property(TARGET BasicTextureDemo PROPERTY CXX_STANDARD 20)

# Link libraries
target_link_libraries(BasicTextureDemo PRIVATE AquaCore AquaEngine)

# Include directories
target_include_directories(BasicTextureDemo PRIVATE 
    ${CMAKE_SOURCE_DIR}/Core
    ${CMAKE_SOURCE_DIR}/Engine
)

# Set output directory
set_target_properties(BasicTextureDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples/$<CONFIG>
)