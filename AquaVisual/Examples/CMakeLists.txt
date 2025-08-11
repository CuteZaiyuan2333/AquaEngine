# AquaVisual Examples

# Basic example (不需要GLFW)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Basic/CMakeLists.txt")
    add_subdirectory(Basic)
endif()

# WindowTest example
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/WindowTest/CMakeLists.txt")
    add_subdirectory(WindowTest)
endif()

# BasicTriangle example (需要GLFW，暂时跳过)
# if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/BasicTriangle/CMakeLists.txt")
#     add_subdirectory(BasicTriangle)
# endif()

# Integration example (需要GLFW，暂时跳过)
# if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Integration/CMakeLists.txt")
#     add_subdirectory(Integration)
# endif()