cmake_minimum_required(VERSION 3.16)

project(WindowTest)

# Create executable
add_executable(WindowTest WindowTest.cpp)

# Link Windows libraries
target_link_libraries(WindowTest user32 gdi32)

# Set output directory
set_target_properties(WindowTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)