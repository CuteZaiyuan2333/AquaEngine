# Core module CMakeLists.txt

# Collect all source files
file(GLOB_RECURSE CORE_SOURCES
    "Math/*.cpp"
    "Math/*.h"
    "Renderer/*.cpp"
    "Renderer/*.h"
    "Platform/*.cpp"
    "Platform/*.h"
    "Memory/*.cpp"
    "Memory/*.h"
    "Utils/*.cpp"
    "Utils/*.h"
)

# Create Core library
add_library(AquaCore STATIC ${CORE_SOURCES})

# Set target properties
set_target_properties(AquaCore PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Find GLFW
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
    # Fallback to manual path if package not found
    if(WIN32)
        set(GLFW_ROOT "D:/WinApplication/glfw/glfw-3.4.bin.WIN64" CACHE PATH "GLFW root directory")
        set(GLFW_INCLUDE_DIR "${GLFW_ROOT}/include")
        set(GLFW_LIBRARY_DIR "${GLFW_ROOT}/lib-vc2022")
        
        # Create imported target for GLFW
        add_library(glfw STATIC IMPORTED)
        set_target_properties(glfw PROPERTIES
            IMPORTED_LOCATION "${GLFW_LIBRARY_DIR}/glfw3.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${GLFW_INCLUDE_DIR}"
        )
    else()
        message(FATAL_ERROR "GLFW not found. Please install GLFW or set GLFW_ROOT")
    endif()
endif()

# Link dependencies
target_link_libraries(AquaCore 
    PUBLIC 
        Vulkan::Vulkan
        $<IF:$<TARGET_EXISTS:glfw>,glfw,$<IF:$<TARGET_EXISTS:glfw3>,glfw3,glfw>>
)

# Include directories
target_include_directories(AquaCore 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${Vulkan_INCLUDE_DIRS}
)

# Compile definitions
target_compile_definitions(AquaCore 
    PUBLIC
        $<$<CONFIG:Debug>:AQUA_DEBUG>
        $<$<CONFIG:Release>:AQUA_RELEASE>
)