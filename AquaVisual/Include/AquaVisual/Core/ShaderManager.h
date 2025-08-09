#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#ifdef AQUA_HAS_VULKAN
#include <vulkan/vulkan.h>
#endif

namespace AquaVisual {

enum class ShaderType {
    Vertex,
    Fragment,
    Geometry,
    Compute
};

struct ShaderSource {
    ShaderType type;
    std::string source;
    std::string entryPoint = "main";
    std::vector<uint32_t> spirvCode;
};

class ShaderModule {
public:
    ShaderModule() = default;
    ~ShaderModule();

    bool LoadFromFile(const std::string& filepath, ShaderType type);
    bool LoadFromSource(const std::string& source, ShaderType type);
    bool CompileToSpirv();
    
    const std::vector<uint32_t>& GetSpirv() const { return m_spirvCode; }
    ShaderType GetType() const { return m_type; }
    const std::string& GetEntryPoint() const { return m_entryPoint; }

#ifdef AQUA_HAS_VULKAN
    VkShaderModule GetVulkanModule() const { return m_vulkanModule; }
    bool CreateVulkanModule(VkDevice device);
    void DestroyVulkanModule(VkDevice device);
#endif

private:
    ShaderType m_type;
    std::string m_source;
    std::string m_entryPoint = "main";
    std::vector<uint32_t> m_spirvCode;

#ifdef AQUA_HAS_VULKAN
    VkShaderModule m_vulkanModule = VK_NULL_HANDLE;
#endif
};

class ShaderProgram {
public:
    ShaderProgram() = default;
    ~ShaderProgram() = default;

    bool AddShader(std::shared_ptr<ShaderModule> shader);
    bool Link();
    
    const std::vector<std::shared_ptr<ShaderModule>>& GetShaders() const { return m_shaders; }

#ifdef AQUA_HAS_VULKAN
    std::vector<VkPipelineShaderStageCreateInfo> GetVulkanStages() const;
#endif

private:
    std::vector<std::shared_ptr<ShaderModule>> m_shaders;
    bool m_linked = false;
};

class ShaderManager {
public:
    static ShaderManager& Instance();

    std::shared_ptr<ShaderModule> LoadShader(const std::string& name, 
                                            const std::string& filepath, 
                                            ShaderType type);
    
    std::shared_ptr<ShaderModule> CreateShaderFromSource(const std::string& name,
                                                        const std::string& source,
                                                        ShaderType type);

    std::shared_ptr<ShaderProgram> CreateProgram(const std::string& name);
    std::shared_ptr<ShaderProgram> GetProgram(const std::string& name);

    void ReloadShader(const std::string& name);
    void ReloadAllShaders();

    // Hot reload support
    void EnableHotReload(bool enable) { m_hotReloadEnabled = enable; }
    void CheckForChanges();

private:
    ShaderManager() = default;
    
    std::unordered_map<std::string, std::shared_ptr<ShaderModule>> m_shaders;
    std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> m_programs;
    std::unordered_map<std::string, std::string> m_shaderPaths;
    std::unordered_map<std::string, uint64_t> m_lastModified;
    
    bool m_hotReloadEnabled = false;

    bool CompileGlslToSpirv(const std::string& source, ShaderType type, std::vector<uint32_t>& spirv);
    std::string ReadFile(const std::string& filepath);
    uint64_t GetFileModificationTime(const std::string& filepath);
};

} // namespace AquaVisual