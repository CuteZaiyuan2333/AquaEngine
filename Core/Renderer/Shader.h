#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <memory>

namespace Aqua {
namespace Renderer {

class Shader {
public:
    enum class Type {
        Vertex,
        Fragment,
        Geometry,
        Compute
    };
    
    Shader(VkDevice device = VK_NULL_HANDLE);
    ~Shader();
    
    // Load shader from GLSL file
    bool LoadFromFile(const std::string& filename, Type type);
    
    // Create shader from GLSL source code
    bool CreateFromSource(const std::string& source, Type type);
    
    // Create shader from SPIR-V bytecode
    bool CreateFromSpirv(const std::vector<uint32_t>& spirvCode, Type type);
    
    // Load shader from SPIR-V file
    bool LoadFromSpirv(const std::string& filename, Type type);
    
    // Get Vulkan shader stage flags
    VkShaderStageFlagBits GetStageFlags() const;
    
    // Get shader module handle
    VkShaderModule GetShaderModule() const { return m_shaderModule; }
    
    // Get shader type
    Type GetType() const { return m_type; }
    
    // Cleanup resources
    void Cleanup();
    
private:
    VkDevice m_device;
    VkShaderModule m_shaderModule;
    Type m_type;
    
    // Compile GLSL to SPIR-V (requires glslang or shaderc)
    std::vector<uint32_t> CompileGLSL(const std::string& source, Type type);
    
    // Create Vulkan shader module from SPIR-V
    bool CreateShaderModule(const std::vector<uint32_t>& spirvCode);
    
    // Read file contents
    std::string ReadFile(const std::string& filename);
};

class ShaderProgram {
public:
    ShaderProgram(VkDevice device = VK_NULL_HANDLE);
    ~ShaderProgram();
    
    // Add shader to the program
    bool AddShader(const Shader& shader);
    
    // Get shader stages for pipeline creation
    std::vector<VkPipelineShaderStageCreateInfo> GetShaderStages() const;
    
    // Cleanup resources
    void Cleanup();
    
private:
    VkDevice m_device;
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
    std::vector<VkShaderModule> m_shaderModules;
};

} // namespace Renderer
} // namespace Aqua