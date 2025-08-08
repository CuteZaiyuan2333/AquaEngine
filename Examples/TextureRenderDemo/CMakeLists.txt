# TextureRenderDemo CMakeLists.txt
add_executable(TextureRenderDemo main.cpp)

# Set C++ standard
set_property(TARGET TextureRenderDemo PROPERTY CXX_STANDARD 20)

# Link libraries
target_link_libraries(TextureRenderDemo PRIVATE AquaCore AquaEngine)

# Include directories
target_include_directories(TextureRenderDemo PRIVATE 
    ${CMAKE_SOURCE_DIR}/Core
    ${CMAKE_SOURCE_DIR}/Engine
)

# Set output directory
set_target_properties(TextureRenderDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples/$<CONFIG>
)