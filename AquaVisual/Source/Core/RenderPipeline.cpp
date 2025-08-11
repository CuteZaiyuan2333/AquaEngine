#include "AquaVisual/Core/RenderPipeline.h"
#include <array>
#include <iostream>

#ifdef AQUA_HAS_VULKAN
#include <vulkan/vulkan.h>
#endif

namespace AquaVisual {

// RenderPipeline Implementation
RenderPipeline::~RenderPipeline() { Destroy(); }

bool RenderPipeline::Create(const PipelineCreateInfo &createInfo) {
  m_createInfo = createInfo;
  m_name = createInfo.name;

  if (!createInfo.shaderProgram) {
    std::cerr << "Pipeline creation failed: No shader program provided"
              << std::endl;
    return false;
  }

  std::cout << "Created render pipeline: " << m_name << std::endl;
  return true;
}

void RenderPipeline::Destroy() {
#ifdef AQUA_HAS_VULKAN
  DestroyVulkanPipeline(m_device);
#endif
}

#ifdef AQUA_HAS_VULKAN
bool RenderPipeline::CreateVulkanPipeline(VkDevice device,
                                          VkRenderPass renderPass) {
  m_device = device;

  // Get shader stages
  auto shaderStages = m_createInfo.shaderProgram->GetVulkanStages();

  // Vertex input state
  std::vector<VkVertexInputBindingDescription> bindingDescriptions;
  std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

  for (const auto &binding : m_createInfo.vertexInput.bindings) {
    VkVertexInputBindingDescription bindingDesc{};
    bindingDesc.binding = binding.binding;
    bindingDesc.stride = binding.stride;
    bindingDesc.inputRate = static_cast<VkVertexInputRate>(binding.inputRate);
    bindingDescriptions.push_back(bindingDesc);
  }

  for (const auto &attribute : m_createInfo.vertexInput.attributes) {
    VkVertexInputAttributeDescription attributeDesc{};
    attributeDesc.binding = attribute.binding;
    attributeDesc.location = attribute.location;
    attributeDesc.format = static_cast<VkFormat>(attribute.format);
    attributeDesc.offset = attribute.offset;
    attributeDescriptions.push_back(attributeDesc);
  }

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount =
      static_cast<uint32_t>(bindingDescriptions.size());
  vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
  vertexInputInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescriptions.size());
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

  // Input assembly state
  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  // Viewport state (dynamic)
  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;

  // Rasterization state
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = m_createInfo.rasterization.depthClampEnable;
  rasterizer.rasterizerDiscardEnable =
      m_createInfo.rasterization.rasterizerDiscardEnable;
  rasterizer.polygonMode =
      static_cast<VkPolygonMode>(m_createInfo.rasterization.polygonMode);
  rasterizer.lineWidth = m_createInfo.rasterization.lineWidth;
  rasterizer.cullMode =
      static_cast<VkCullModeFlags>(m_createInfo.rasterization.cullMode);
  rasterizer.frontFace =
      static_cast<VkFrontFace>(m_createInfo.rasterization.frontFace);
  rasterizer.depthBiasEnable = m_createInfo.rasterization.depthBiasEnable;

  // Multisample state
  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable =
      m_createInfo.multisample.sampleShadingEnable;
  multisampling.rasterizationSamples = static_cast<VkSampleCountFlagBits>(
      m_createInfo.multisample.rasterizationSamples);

  // Depth stencil state
  VkPipelineDepthStencilStateCreateInfo depthStencil{};
  depthStencil.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = m_createInfo.depthStencil.depthTestEnable;
  depthStencil.depthWriteEnable = m_createInfo.depthStencil.depthWriteEnable;
  depthStencil.depthCompareOp =
      static_cast<VkCompareOp>(m_createInfo.depthStencil.depthCompareOp);
  depthStencil.depthBoundsTestEnable =
      m_createInfo.depthStencil.depthBoundsTestEnable;
  depthStencil.stencilTestEnable = m_createInfo.depthStencil.stencilTestEnable;

  // Color blend state
  std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
  for (const auto &attachment : m_createInfo.colorBlend.attachments) {
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = attachment.colorWriteMask;
    colorBlendAttachment.blendEnable = attachment.blendEnable;
    colorBlendAttachment.srcColorBlendFactor =
        static_cast<VkBlendFactor>(attachment.srcColorBlendFactor);
    colorBlendAttachment.dstColorBlendFactor =
        static_cast<VkBlendFactor>(attachment.dstColorBlendFactor);
    colorBlendAttachment.colorBlendOp =
        static_cast<VkBlendOp>(attachment.colorBlendOp);
    colorBlendAttachment.srcAlphaBlendFactor =
        static_cast<VkBlendFactor>(attachment.srcAlphaBlendFactor);
    colorBlendAttachment.dstAlphaBlendFactor =
        static_cast<VkBlendFactor>(attachment.dstAlphaBlendFactor);
    colorBlendAttachment.alphaBlendOp =
        static_cast<VkBlendOp>(attachment.alphaBlendOp);
    colorBlendAttachments.push_back(colorBlendAttachment);
  }

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = m_createInfo.colorBlend.logicOpEnable;
  colorBlending.logicOp =
      static_cast<VkLogicOp>(m_createInfo.colorBlend.logicOp);
  colorBlending.attachmentCount =
      static_cast<uint32_t>(colorBlendAttachments.size());
  colorBlending.pAttachments = colorBlendAttachments.data();
  colorBlending.blendConstants[0] = m_createInfo.colorBlend.blendConstants[0];
  colorBlending.blendConstants[1] = m_createInfo.colorBlend.blendConstants[1];
  colorBlending.blendConstants[2] = m_createInfo.colorBlend.blendConstants[2];
  colorBlending.blendConstants[3] = m_createInfo.colorBlend.blendConstants[3];

  // Dynamic state
  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  // Pipeline layout (empty for now)
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pushConstantRangeCount = 0;

  if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
                             &m_pipelineLayout) != VK_SUCCESS) {
    std::cerr << "Failed to create pipeline layout" << std::endl;
    return false;
  }

  // Create graphics pipeline
  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
  pipelineInfo.pStages = shaderStages.data();
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = m_pipelineLayout;
  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = m_createInfo.subpass;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                nullptr, &m_pipeline) != VK_SUCCESS) {
    std::cerr << "Failed to create graphics pipeline" << std::endl;
    return false;
  }

  std::cout << "Created Vulkan pipeline: " << m_name << std::endl;
  return true;
}

void RenderPipeline::DestroyVulkanPipeline(VkDevice device) {
  if (device != VK_NULL_HANDLE) {
    if (m_pipeline != VK_NULL_HANDLE) {
      vkDestroyPipeline(device, m_pipeline, nullptr);
      m_pipeline = VK_NULL_HANDLE;
    }
    if (m_pipelineLayout != VK_NULL_HANDLE) {
      vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
      m_pipelineLayout = VK_NULL_HANDLE;
    }
  }
}
#endif

// PipelineManager Implementation
PipelineManager &PipelineManager::Instance() {
  static PipelineManager instance;
  return instance;
}

std::shared_ptr<RenderPipeline>
PipelineManager::CreatePipeline(const PipelineCreateInfo &createInfo) {
  auto pipeline = std::make_shared<RenderPipeline>();
  if (pipeline->Create(createInfo)) {
    m_pipelines[createInfo.name] = pipeline;
    return pipeline;
  }
  return nullptr;
}

std::shared_ptr<RenderPipeline>
PipelineManager::GetPipeline(const std::string &name) {
  auto it = m_pipelines.find(name);
  return (it != m_pipelines.end()) ? it->second : nullptr;
}

void PipelineManager::DestroyPipeline(const std::string &name) {
  auto it = m_pipelines.find(name);
  if (it != m_pipelines.end()) {
    m_pipelines.erase(it);
  }
}

void PipelineManager::DestroyAllPipelines() { m_pipelines.clear(); }

// Preset pipeline configurations
PipelineCreateInfo PipelineManager::CreateBasicPipelineInfo() {
  PipelineCreateInfo info;
  info.name = "Basic";

#ifdef AQUA_HAS_VULKAN
  // Set basic rasterization state
  info.rasterization.polygonMode = VK_POLYGON_MODE_FILL;
  info.rasterization.cullMode = VK_CULL_MODE_BACK_BIT;
  info.rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  info.rasterization.lineWidth = 1.0f;

  // Set multisampling
  info.multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  // Set depth testing
  info.depthStencil.depthTestEnable = true;
  info.depthStencil.depthWriteEnable = true;
  info.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;

  // Set color blending
  ColorBlendAttachment colorAttachment;
  colorAttachment.blendEnable = false;
  colorAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  info.colorBlend.attachments.push_back(colorAttachment);
#else
  // Placeholder values for non-Vulkan builds
  info.rasterization.polygonMode = 0;
  info.rasterization.cullMode = 2;
  info.rasterization.frontFace = 1;
  info.rasterization.lineWidth = 1.0f;

  info.multisample.rasterizationSamples = 1;

  info.depthStencil.depthTestEnable = true;
  info.depthStencil.depthWriteEnable = true;
  info.depthStencil.depthCompareOp = 1;

  ColorBlendAttachment colorAttachment;
  colorAttachment.blendEnable = false;
  colorAttachment.colorWriteMask = 0x0000000F;
  info.colorBlend.attachments.push_back(colorAttachment);
#endif

  return info;
}

PipelineCreateInfo PipelineManager::CreateUnlitPipelineInfo() {
  auto info = CreateBasicPipelineInfo();
  info.name = "Unlit";
  info.depthStencil.depthTestEnable = false;
  return info;
}

PipelineCreateInfo PipelineManager::CreatePBRPipelineInfo() {
  auto info = CreateBasicPipelineInfo();
  info.name = "PBR";

#ifdef AQUA_HAS_VULKAN
  // Enable transparency blending
  info.colorBlend.attachments[0].blendEnable = true;
  info.colorBlend.attachments[0].srcColorBlendFactor =
      VK_BLEND_FACTOR_SRC_ALPHA;
  info.colorBlend.attachments[0].dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  info.colorBlend.attachments[0].colorBlendOp = VK_BLEND_OP_ADD;
  info.colorBlend.attachments[0].srcAlphaBlendFactor =
      VK_BLEND_FACTOR_SRC_ALPHA;
  info.colorBlend.attachments[0].dstAlphaBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  info.colorBlend.attachments[0].alphaBlendOp = VK_BLEND_OP_ADD;
#else
  // Placeholder values for non-Vulkan builds
  info.colorBlend.attachments[0].blendEnable = true;
  info.colorBlend.attachments[0].srcColorBlendFactor = 6;
  info.colorBlend.attachments[0].dstColorBlendFactor = 7;
  info.colorBlend.attachments[0].colorBlendOp = 0;
  info.colorBlend.attachments[0].srcAlphaBlendFactor = 6;
  info.colorBlend.attachments[0].dstAlphaBlendFactor = 7;
  info.colorBlend.attachments[0].alphaBlendOp = 0;
#endif

  return info;
}

// Vertex format implementations

VertexInputDescription SimpleVertex::GetInputDescription() {
  VertexInputDescription description;

  // Binding description
  VertexInputDescription::Binding binding;
  binding.binding = 0;
  binding.stride = sizeof(SimpleVertex);
#ifdef AQUA_HAS_VULKAN
  binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
#else
  binding.inputRate = 0;
#endif
  description.bindings.push_back(binding);

  // Position
  VertexInputDescription::Attribute posAttr;
  posAttr.binding = 0;
  posAttr.location = 0;
#ifdef AQUA_HAS_VULKAN
  posAttr.format = VK_FORMAT_R32G32B32_SFLOAT;
#else
  posAttr.format = 106;
#endif
  posAttr.offset = offsetof(SimpleVertex, position);
  description.attributes.push_back(posAttr);

  // Color
  VertexInputDescription::Attribute colorAttr;
  colorAttr.binding = 0;
  colorAttr.location = 1;
#ifdef AQUA_HAS_VULKAN
  colorAttr.format = VK_FORMAT_R32G32B32_SFLOAT;
#else
  colorAttr.format = 106;
#endif
  colorAttr.offset = offsetof(SimpleVertex, color);
  description.attributes.push_back(colorAttr);

  return description;
}

} // namespace AquaVisual