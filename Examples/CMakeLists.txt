# Examples CMakeLists.txt

# Add subdirectories for each example
add_subdirectory(HelloTriangle)
add_subdirectory(TextureDemo)
add_subdirectory(SimpleTextureDemo)
add_subdirectory(TextureShowcase)
add_subdirectory(SimpleTextureShow)
add_subdirectory(TextureRenderDemo)
add_subdirectory(BasicTextureDemo)
add_subdirectory(TextureWindowDemo)
add_subdirectory(TextureRenderingDemo)
add_subdirectory(SimpleTextureRender)

# TextureTest example
add_executable(TextureTest TextureTest/main.cpp)

# Set C++ standard
set_property(TARGET TextureTest PROPERTY CXX_STANDARD 20)

# Link libraries
target_link_libraries(TextureTest PRIVATE AquaCore AquaEngine)

# Include directories
target_include_directories(TextureTest PRIVATE 
    ${CMAKE_SOURCE_DIR}/Core
    ${CMAKE_SOURCE_DIR}/Engine
)

# Set output directory
set_target_properties(TextureTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples/$<CONFIG>
)