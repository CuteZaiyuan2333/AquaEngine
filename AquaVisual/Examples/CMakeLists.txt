# AquaVisual Examples

# Basic example
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Basic/CMakeLists.txt")
    add_subdirectory(Basic)
endif()

# BasicTriangle example
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/BasicTriangle/CMakeLists.txt")
    add_subdirectory(BasicTriangle)
endif()

# Integration example
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Integration/CMakeLists.txt")
    add_subdirectory(Integration)
endif()