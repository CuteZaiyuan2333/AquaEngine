#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace Aqua {
namespace Renderer {

Shader::Shader(VkDevice device)
    : m_device(device), m_shaderModule(VK_NULL_HANDLE), m_type(Type::Vertex) {
}

Shader::~Shader() {
    Cleanup();
}

bool Shader::LoadFromFile(const std::string& filename, Type type) {
    std::string source = ReadFile(filename);
    if (source.empty()) {
        std::cerr << "Failed to read shader file: " << filename << std::endl;
        return false;
    }
    
    return CreateFromSource(source, type);
}

bool Shader::CreateFromSource(const std::string& source, Type type) {
    m_type = type;
    
    // Compile GLSL to SPIR-V
    std::vector<uint32_t> spirvCode = CompileGLSL(source, type);
    if (spirvCode.empty()) {
        std::cerr << "GLSL compilation failed" << std::endl;
        return false;
    }
    
    return CreateFromSpirv(spirvCode, type);
}

bool Shader::CreateFromSpirv(const std::vector<uint32_t>& spirvCode, Type type) {
    m_type = type;
    return CreateShaderModule(spirvCode);
}

VkShaderStageFlagBits Shader::GetStageFlags() const {
    switch (m_type) {
        case Type::Vertex:   return VK_SHADER_STAGE_VERTEX_BIT;
        case Type::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
        case Type::Geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
        case Type::Compute:  return VK_SHADER_STAGE_COMPUTE_BIT;
        default:             return VK_SHADER_STAGE_VERTEX_BIT;
    }
}

void Shader::Cleanup() {
    if (m_shaderModule != VK_NULL_HANDLE) {
        vkDestroyShaderModule(m_device, m_shaderModule, nullptr);
        m_shaderModule = VK_NULL_HANDLE;
    }
}

std::vector<uint32_t> Shader::CompileGLSL(const std::string& source, Type type) {
    // Note: This requires integration with glslang or shaderc library to compile GLSL
    // For simplicity, we return empty vector for now, actual projects need to implement GLSL compilation
    std::cerr << "GLSL compilation not implemented yet, please use pre-compiled SPIR-V files" << std::endl;
    return {};
}

bool Shader::LoadFromSpirv(const std::string& filename, Type type) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open SPIR-V file: " << filename << std::endl;
        return false;
    }
    
    size_t fileSize = (size_t)file.tellg();
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
    
    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    file.close();
    
    return CreateFromSpirv(buffer, type);
}

bool Shader::CreateShaderModule(const std::vector<uint32_t>& spirvCode) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirvCode.size() * sizeof(uint32_t);
    createInfo.pCode = spirvCode.data();
    
    if (vkCreateShaderModule(m_device, &createInfo, nullptr, &m_shaderModule) != VK_SUCCESS) {
        std::cerr << "Failed to create shader module" << std::endl;
        return false;
    }
    
    std::cout << "Shader module created successfully" << std::endl;
    return true;
}

std::string Shader::ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return "";
    }
    
    size_t fileSize = (size_t)file.tellg();
    std::string buffer(fileSize, ' ');
    
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    
    return buffer;
}

// ShaderProgram implementation
ShaderProgram::ShaderProgram(VkDevice device)
    : m_device(device) {
}

ShaderProgram::~ShaderProgram() {
    Cleanup();
}

bool ShaderProgram::AddShader(const Shader& shader) {
    VkPipelineShaderStageCreateInfo shaderStageInfo{};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = shader.GetStageFlags();
    shaderStageInfo.module = shader.GetShaderModule();
    shaderStageInfo.pName = "main"; // Shader entry point function name
    
    m_shaderStages.push_back(shaderStageInfo);
    m_shaderModules.push_back(shader.GetShaderModule());
    
    std::cout << "Shader added to program successfully" << std::endl;
    return true;
}

std::vector<VkPipelineShaderStageCreateInfo> ShaderProgram::GetShaderStages() const {
    return m_shaderStages;
}

void ShaderProgram::Cleanup() {
    // Note: Shader module cleanup is handled by Shader class
    // Only clean up local data here
    m_shaderStages.clear();
    m_shaderModules.clear();
}

} // namespace Renderer
} // namespace Aqua