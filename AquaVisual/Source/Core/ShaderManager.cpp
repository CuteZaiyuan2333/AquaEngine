#include "AquaVisual/Core/ShaderManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#ifdef AQUA_HAS_VULKAN
// Note: Actual project needs to link shaderc library to compile GLSL
// This provides basic framework, actual compilation requires shaderc
#endif

namespace AquaVisual {

// ShaderModule Implementation
ShaderModule::~ShaderModule() {
#ifdef AQUA_HAS_VULKAN
    // VulkanModule should be managed and destroyed by VulkanRenderer
#endif
}

bool ShaderModule::LoadFromFile(const std::string& filepath, ShaderType type) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    m_source = buffer.str();
    m_type = type;

    std::cout << "Loaded shader from file: " << filepath << std::endl;
    return CompileToSpirv();
}

bool ShaderModule::LoadFromSource(const std::string& source, ShaderType type) {
    m_source = source;
    m_type = type;
    return CompileToSpirv();
}

bool ShaderModule::CompileToSpirv() {
    // This is a simplified implementation, actual project needs to use shaderc library
    // Now we assume we have pre-compiled SPIRV or use placeholder
    
    std::cout << "Compiling shader to SPIRV (placeholder implementation)" << std::endl;
    
    // Placeholder: generate some fake SPIRV data
    // In actual implementation, this should call shaderc compiler
    m_spirvCode = {
        0x07230203, // SPIRV magic number
        0x00010000, // version
        0x00080001, // generator
        0x0000000D, // instruction count
        0x00000000  // reserved
    };
    
    return true;
}

#ifdef AQUA_HAS_VULKAN
bool ShaderModule::CreateVulkanModule(VkDevice device) {
    if (m_spirvCode.empty()) {
        std::cerr << "No SPIRV code available for Vulkan module creation" << std::endl;
        return false;
    }

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = m_spirvCode.size() * sizeof(uint32_t);
    createInfo.pCode = m_spirvCode.data();

    if (vkCreateShaderModule(device, &createInfo, nullptr, &m_vulkanModule) != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan shader module" << std::endl;
        return false;
    }

    return true;
}

void ShaderModule::DestroyVulkanModule(VkDevice device) {
    if (m_vulkanModule != VK_NULL_HANDLE) {
        vkDestroyShaderModule(device, m_vulkanModule, nullptr);
        m_vulkanModule = VK_NULL_HANDLE;
    }
}
#endif

// ShaderProgram Implementation
bool ShaderProgram::AddShader(std::shared_ptr<ShaderModule> shader) {
    if (!shader) {
        return false;
    }
    
    m_shaders.push_back(shader);
    m_linked = false; // Need to re-link
    return true;
}

bool ShaderProgram::Link() {
    if (m_shaders.empty()) {
        std::cerr << "No shaders to link" << std::endl;
        return false;
    }

    // Validate shader combination validity
    bool hasVertex = false, hasFragment = false;
    for (const auto& shader : m_shaders) {
        if (shader->GetType() == ShaderType::Vertex) hasVertex = true;
        if (shader->GetType() == ShaderType::Fragment) hasFragment = true;
    }

    if (!hasVertex || !hasFragment) {
        std::cerr << "Shader program must have at least vertex and fragment shaders" << std::endl;
        return false;
    }

    m_linked = true;
    std::cout << "Shader program linked successfully with " << m_shaders.size() << " shaders" << std::endl;
    return true;
}

#ifdef AQUA_HAS_VULKAN
std::vector<VkPipelineShaderStageCreateInfo> ShaderProgram::GetVulkanStages() const {
    std::vector<VkPipelineShaderStageCreateInfo> stages;
    
    for (const auto& shader : m_shaders) {
        VkPipelineShaderStageCreateInfo stageInfo{};
        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        
        switch (shader->GetType()) {
            case ShaderType::Vertex:
                stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case ShaderType::Fragment:
                stageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            case ShaderType::Geometry:
                stageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
                break;
            case ShaderType::Compute:
                stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
                break;
        }
        
        stageInfo.module = shader->GetVulkanModule();
        stageInfo.pName = shader->GetEntryPoint().c_str();
        
        stages.push_back(stageInfo);
    }
    
    return stages;
}
#endif

// ShaderManager Implementation
ShaderManager& ShaderManager::Instance() {
    static ShaderManager instance;
    return instance;
}

std::shared_ptr<ShaderModule> ShaderManager::LoadShader(const std::string& name, 
                                                       const std::string& filepath, 
                                                       ShaderType type) {
    auto shader = std::make_shared<ShaderModule>();
    if (shader->LoadFromFile(filepath, type)) {
        m_shaders[name] = shader;
        m_shaderPaths[name] = filepath;
        m_lastModified[name] = GetFileModificationTime(filepath);
        return shader;
    }
    return nullptr;
}

std::shared_ptr<ShaderModule> ShaderManager::CreateShaderFromSource(const std::string& name,
                                                                   const std::string& source,
                                                                   ShaderType type) {
    auto shader = std::make_shared<ShaderModule>();
    if (shader->LoadFromSource(source, type)) {
        m_shaders[name] = shader;
        return shader;
    }
    return nullptr;
}

std::shared_ptr<ShaderProgram> ShaderManager::CreateProgram(const std::string& name) {
    auto program = std::make_shared<ShaderProgram>();
    m_programs[name] = program;
    return program;
}

std::shared_ptr<ShaderProgram> ShaderManager::GetProgram(const std::string& name) {
    auto it = m_programs.find(name);
    return (it != m_programs.end()) ? it->second : nullptr;
}

void ShaderManager::ReloadShader(const std::string& name) {
    auto it = m_shaderPaths.find(name);
    if (it != m_shaderPaths.end()) {
        auto shaderIt = m_shaders.find(name);
        if (shaderIt != m_shaders.end()) {
            // Reload shader
            std::cout << "Reloading shader: " << name << std::endl;
            // Here needs recompilation and update
        }
    }
}

void ShaderManager::ReloadAllShaders() {
    for (const auto& pair : m_shaderPaths) {
        ReloadShader(pair.first);
    }
}

void ShaderManager::CheckForChanges() {
    if (!m_hotReloadEnabled) return;

    for (const auto& pair : m_shaderPaths) {
        const std::string& name = pair.first;
        const std::string& path = pair.second;
        
        uint64_t currentTime = GetFileModificationTime(path);
        if (currentTime > m_lastModified[name]) {
            std::cout << "Detected change in shader: " << name << std::endl;
            ReloadShader(name);
            m_lastModified[name] = currentTime;
        }
    }
}

std::string ShaderManager::ReadFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

uint64_t ShaderManager::GetFileModificationTime(const std::string& filepath) {
    try {
        auto ftime = std::filesystem::last_write_time(filepath);
        return std::chrono::duration_cast<std::chrono::seconds>(ftime.time_since_epoch()).count();
    } catch (...) {
        return 0;
    }
}

bool ShaderManager::CompileGlslToSpirv(const std::string& source, ShaderType type, std::vector<uint32_t>& spirv) {
    // Actual implementation needs to use shaderc library
    // This is placeholder implementation
    std::cout << "Compiling GLSL to SPIRV (placeholder)" << std::endl;
    
    // Generate placeholder SPIRV
    spirv = {
        0x07230203, // SPIRV magic number
        0x00010000, // version
        0x00080001, // generator
        0x0000000D, // instruction count
        0x00000000  // reserved
    };
    
    return true;
}

} // namespace AquaVisual