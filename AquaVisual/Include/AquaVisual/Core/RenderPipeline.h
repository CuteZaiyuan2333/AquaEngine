#pragma once

#include "AquaVisual/Core/ShaderManager.h"
#include <memory>
#include <vector>

#ifdef AQUA_HAS_VULKAN
#include <vulkan/vulkan.h>
#endif

namespace AquaVisual {

struct VertexInputDescription {
    struct Attribute {
        uint32_t location;
        uint32_t binding;
        uint32_t format; // VkFormat
        uint32_t offset;
    };

    struct Binding {
        uint32_t binding;
        uint32_t stride;
        uint32_t inputRate; // VkVertexInputRate
    };

    std::vector<Attribute> attributes;
    std::vector<Binding> bindings;
};

struct RasterizationState {
    bool depthClampEnable = false;
    bool rasterizerDiscardEnable = false;
    uint32_t polygonMode = 0; // VK_POLYGON_MODE_FILL
    uint32_t cullMode = 0;    // VK_CULL_MODE_BACK_BIT
    uint32_t frontFace = 0;   // VK_FRONT_FACE_CLOCKWISE
    bool depthBiasEnable = false;
    float depthBiasConstantFactor = 0.0f;
    float depthBiasClamp = 0.0f;
    float depthBiasSlopeFactor = 0.0f;
    float lineWidth = 1.0f;
};

struct MultisampleState {
    uint32_t rasterizationSamples = 1; // VK_SAMPLE_COUNT_1_BIT
    bool sampleShadingEnable = false;
    float minSampleShading = 1.0f;
    bool alphaToCoverageEnable = false;
    bool alphaToOneEnable = false;
};

struct DepthStencilState {
    bool depthTestEnable = true;
    bool depthWriteEnable = true;
    uint32_t depthCompareOp = 1; // VK_COMPARE_OP_LESS
    bool depthBoundsTestEnable = false;
    bool stencilTestEnable = false;
    float minDepthBounds = 0.0f;
    float maxDepthBounds = 1.0f;
};

struct ColorBlendAttachment {
    bool blendEnable = false;
    uint32_t srcColorBlendFactor = 1; // VK_BLEND_FACTOR_ONE
    uint32_t dstColorBlendFactor = 0; // VK_BLEND_FACTOR_ZERO
    uint32_t colorBlendOp = 0;        // VK_BLEND_OP_ADD
    uint32_t srcAlphaBlendFactor = 1; // VK_BLEND_FACTOR_ONE
    uint32_t dstAlphaBlendFactor = 0; // VK_BLEND_FACTOR_ZERO
    uint32_t alphaBlendOp = 0;        // VK_BLEND_OP_ADD
    uint32_t colorWriteMask = 0xF;    // VK_COLOR_COMPONENT_R_BIT | G | B | A
};

struct ColorBlendState {
    bool logicOpEnable = false;
    uint32_t logicOp = 0; // VK_LOGIC_OP_COPY
    std::vector<ColorBlendAttachment> attachments;
    float blendConstants[4] = {0.0f, 0.0f, 0.0f, 0.0f};
};

struct PipelineCreateInfo {
    std::shared_ptr<ShaderProgram> shaderProgram;
    VertexInputDescription vertexInput;
    RasterizationState rasterization;
    MultisampleState multisample;
    DepthStencilState depthStencil;
    ColorBlendState colorBlend;
    
    uint32_t renderPassHandle = 0;
    uint32_t subpass = 0;
    
    std::string name;
};

class RenderPipeline {
public:
    RenderPipeline() = default;
    ~RenderPipeline();

    bool Create(const PipelineCreateInfo& createInfo);
    void Destroy();

    const std::string& GetName() const { return m_name; }
    
#ifdef AQUA_HAS_VULKAN
    VkPipeline GetVulkanPipeline() const { return m_pipeline; }
    VkPipelineLayout GetVulkanLayout() const { return m_pipelineLayout; }
    
    bool CreateVulkanPipeline(VkDevice device, VkRenderPass renderPass);
    void DestroyVulkanPipeline(VkDevice device);
#endif

private:
    std::string m_name;
    PipelineCreateInfo m_createInfo;
    
#ifdef AQUA_HAS_VULKAN
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
#endif
};

class PipelineManager {
public:
    static PipelineManager& Instance();

    std::shared_ptr<RenderPipeline> CreatePipeline(const PipelineCreateInfo& createInfo);
    std::shared_ptr<RenderPipeline> GetPipeline(const std::string& name);
    
    void DestroyPipeline(const std::string& name);
    void DestroyAllPipelines();

    // 预设管线配置
    static PipelineCreateInfo CreateBasicPipelineInfo();
    static PipelineCreateInfo CreateUnlitPipelineInfo();
    static PipelineCreateInfo CreatePBRPipelineInfo();

private:
    PipelineManager() = default;
    
    std::unordered_map<std::string, std::shared_ptr<RenderPipeline>> m_pipelines;
};

// Vertex format definitions
struct SimpleVertex {
    float position[3];
    float color[3];
    
    static VertexInputDescription GetInputDescription();
};

} // namespace AquaVisual