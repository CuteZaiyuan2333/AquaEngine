# SimpleTextureShow CMakeLists.txt
add_executable(SimpleTextureShow main.cpp)

# Set C++ standard
set_property(TARGET SimpleTextureShow PROPERTY CXX_STANDARD 20)

# Link libraries
target_link_libraries(SimpleTextureShow PRIVATE AquaCore AquaEngine)

# Include directories
target_include_directories(SimpleTextureShow PRIVATE 
    ${CMAKE_SOURCE_DIR}/Core
    ${CMAKE_SOURCE_DIR}/Engine
)

# Set output directory
set_target_properties(SimpleTextureShow PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples/$<CONFIG>
)