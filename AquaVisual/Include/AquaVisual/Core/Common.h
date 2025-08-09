#pragma once

// Platform detection
#ifdef _WIN32
    #ifdef AQUA_VISUAL_EXPORTS
        #define AQUA_API __declspec(dllexport)
    #else
        #define AQUA_API
    #endif
#else
    #define AQUA_API
#endif

// Compiler specific macros
#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4251) // dll interface needed
#endif

// Common includes
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// Vulkan support detection
#define AQUA_HAS_VULKAN

namespace AquaVisual {
    // Forward declarations
    class Renderer;
    class Window;
    class Camera;
    class Material;
    class Mesh;
    class Texture;
    
    // Common type definitions
    using uint8 = std::uint8_t;
    using uint16 = std::uint16_t;
    using uint32 = std::uint32_t;
    using uint64 = std::uint64_t;
    
    using int8 = std::int8_t;
    using int16 = std::int16_t;
    using int32 = std::int32_t;
    using int64 = std::int64_t;
    
    using float32 = float;
    using float64 = double;
}

#ifdef _MSC_VER
    #pragma warning(pop)
#endif