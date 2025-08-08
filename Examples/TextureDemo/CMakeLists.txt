# TextureDemo CMakeLists.txt
add_executable(TextureDemo main.cpp)

# Set C++ standard
set_property(TARGET TextureDemo PROPERTY CXX_STANDARD 20)

# Link libraries
target_link_libraries(TextureDemo PRIVATE AquaCore AquaEngine)

# Include directories
target_include_directories(TextureDemo PRIVATE 
    ${CMAKE_SOURCE_DIR}/Core
    ${CMAKE_SOURCE_DIR}/Engine
)

# Set output directory
set_target_properties(TextureDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples/$<CONFIG>
)