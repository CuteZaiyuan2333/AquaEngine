# HelloTriangle CMakeLists.txt
add_executable(HelloTriangle main.cpp)

# Set C++ standard
set_property(TARGET HelloTriangle PROPERTY CXX_STANDARD 20)

# Link libraries
target_link_libraries(HelloTriangle PRIVATE AquaCore AquaEngine)

# Include directories
target_include_directories(HelloTriangle PRIVATE 
    ${CMAKE_SOURCE_DIR}/Core
    ${CMAKE_SOURCE_DIR}/Engine
)

# Set output directory
set_target_properties(HelloTriangle PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples/$<CONFIG>
)