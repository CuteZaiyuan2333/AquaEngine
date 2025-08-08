# SimpleTextureDemo CMakeLists.txt
add_executable(SimpleTextureDemo main.cpp)

# Set C++ standard
set_property(TARGET SimpleTextureDemo PROPERTY CXX_STANDARD 20)

# Link libraries
target_link_libraries(SimpleTextureDemo PRIVATE AquaCore AquaEngine)

# Include directories
target_include_directories(SimpleTextureDemo PRIVATE 
    ${CMAKE_SOURCE_DIR}/Core
    ${CMAKE_SOURCE_DIR}/Engine
)

# Set output directory
set_target_properties(SimpleTextureDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples/$<CONFIG>
)