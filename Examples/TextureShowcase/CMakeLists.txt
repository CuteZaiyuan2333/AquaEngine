# TextureShowcase CMakeLists.txt
add_executable(TextureShowcase main.cpp)

# Set C++ standard
set_property(TARGET TextureShowcase PROPERTY CXX_STANDARD 20)

# Link libraries
target_link_libraries(TextureShowcase PRIVATE AquaCore AquaEngine)

# Include directories
target_include_directories(TextureShowcase PRIVATE 
    ${CMAKE_SOURCE_DIR}/Core
    ${CMAKE_SOURCE_DIR}/Engine
)

# Set output directory
set_target_properties(TextureShowcase PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples/$<CONFIG>
)