#pragma once

#include "../Math/Matrix4.h"
#include "../Math/Vector3.h"
#include "Shader.h"
#include "VulkanContext.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>


namespace Aqua {
namespace Renderer {

// Forward declarations
class VulkanContext;

// Constants
const int MAX_FRAMES_IN_FLIGHT = 2;

// Vertex structure
struct Vertex {
  Math::Vector3 position;
  Math::Vector3 color;

  static VkVertexInputBindingDescription GetBindingDescription();
  static std::vector<VkVertexInputAttributeDescription>
  GetAttributeDescriptions();
};

// Uniform Buffer Object
struct UniformBufferObject {
  Math::Matrix4 model;
  Math::Matrix4 view;
  Math::Matrix4 proj;
};

class Renderer {
public:
  Renderer();
  ~Renderer();

  // Non-copyable
  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  bool Initialize(std::shared_ptr<VulkanContext> vulkanContext);
  bool BeginFrame();
  bool EndFrame();
  void RenderTriangle();
  void SetCurrentTexture(VkImageView imageView, VkSampler sampler);
  void Cleanup();

private:
  std::shared_ptr<VulkanContext> m_vulkanContext;

  // Render pass
  VkRenderPass m_renderPass;

  // Pipeline
  VkPipelineLayout m_pipelineLayout;
  VkPipeline m_graphicsPipeline;
  std::unique_ptr<ShaderProgram> m_shaderProgram;

  // Framebuffers
  std::vector<VkFramebuffer> m_swapchainFramebuffers;

  // Command pool and buffers
  VkCommandPool m_commandPool;
  std::vector<VkCommandBuffer> m_commandBuffers;

  // Vertex and index buffers
  VkBuffer m_vertexBuffer;
  VkDeviceMemory m_vertexBufferMemory;
  VkBuffer m_indexBuffer;
  VkDeviceMemory m_indexBufferMemory;

  // Uniform buffers
  std::vector<VkBuffer> m_uniformBuffers;
  std::vector<VkDeviceMemory> m_uniformBuffersMemory;

  // Descriptor sets
  VkDescriptorSetLayout m_descriptorSetLayout;
  VkDescriptorPool m_descriptorPool;
  std::vector<VkDescriptorSet> m_descriptorSets;

  // Synchronization objects
  std::vector<VkSemaphore> m_imageAvailableSemaphores;
  std::vector<VkSemaphore> m_renderFinishedSemaphores;
  std::vector<VkFence> m_inFlightFences;

  // Frame tracking
  uint32_t m_currentFrame;
  uint32_t m_imageIndex;

  // Vertex and index data
  std::vector<Vertex> m_vertices;
  std::vector<uint16_t> m_indices;

  // Current texture
  VkImageView m_currentTextureImageView;
  VkSampler m_currentTextureSampler;
  bool m_needDescriptorUpdate;

  // Default texture (1x1 white pixel)
  VkImage m_defaultTextureImage;
  VkDeviceMemory m_defaultTextureImageMemory;
  VkImageView m_defaultTextureImageView;
  VkSampler m_defaultTextureSampler;

  // Private methods
  bool CreateRenderPass();
  bool CreateDescriptorSetLayout();
  bool CreateGraphicsPipeline();
  bool CreateFramebuffers();
  bool CreateCommandPool();
  bool CreateVertexBuffer();
  bool CreateIndexBuffer();
  bool CreateUniformBuffers();
  bool CreateDescriptorPool();
  bool CreateDescriptorSets();
  bool CreateCommandBuffers();
  bool CreateSyncObjects();
  bool CreateDefaultTexture();

  void UpdateUniformBuffer(uint32_t currentImage);
  void UpdateDescriptorSetsForCurrentFrame();
  void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  void RecreateSwapchain();

  uint32_t FindMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
  void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);
  void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};

} // namespace Renderer
} // namespace Aqua