cmake_minimum_required(VERSION 3.16)
project(TextureLoadTest)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(TextureLoadTest texture_load_test.cpp)

# Include directories
target_include_directories(TextureLoadTest PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParty/STB
)

# Add STB implementation and Texture source
target_sources(TextureLoadTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParty/STB/stb_image_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Source/Resources/Texture.cpp
)

# Define STB_IMAGE_IMPLEMENTATION
target_compile_definitions(TextureLoadTest PRIVATE STB_IMAGE_IMPLEMENTATION)

# Windows specific settings
if(WIN32)
    target_compile_definitions(TextureLoadTest PRIVATE 
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
endif()

# Set working directory for debugging
set_target_properties(TextureLoadTest PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)