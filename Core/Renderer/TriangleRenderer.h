#pragma once

#include "VulkanContext.h"
#include "../Math/Matrix4.h"
#include "../Math/Vector3.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace Aqua {
namespace Renderer {

struct SimpleVertex {
    Math::Vector3 position;
    Math::Vector3 color;

    static VkVertexInputBindingDescription GetBindingDescription();
    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
};

struct SimpleUniformBufferObject {
    Math::Matrix4 model;
    Math::Matrix4 view;
    Math::Matrix4 proj;
};

class TriangleRenderer {
public:
    TriangleRenderer();
    ~TriangleRenderer();

    bool Initialize(std::shared_ptr<VulkanContext> vulkanContext);
    void Cleanup();

    bool BeginFrame();
    void RenderTriangle();
    bool EndFrame();

private:
    static const int MAX_FRAMES_IN_FLIGHT = 2;

    std::shared_ptr<VulkanContext> m_vulkanContext;

    // Vulkan objects
    VkRenderPass m_renderPass;
    VkDescriptorSetLayout m_descriptorSetLayout;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    VkCommandPool m_commandPool;

    // Vertex data
    std::vector<SimpleVertex> m_vertices;
    std::vector<uint16_t> m_indices;
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    VkBuffer m_indexBuffer;
    VkDeviceMemory m_indexBufferMemory;

    // Uniform buffers
    std::vector<VkBuffer> m_uniformBuffers;
    std::vector<VkDeviceMemory> m_uniformBuffersMemory;
    std::vector<void*> m_uniformBuffersMapped;

    // Descriptor sets
    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;

    // Command buffers
    std::vector<VkCommandBuffer> m_commandBuffers;

    // Synchronization objects
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;

    // Framebuffers
    std::vector<VkFramebuffer> m_swapchainFramebuffers;

    // Frame tracking
    uint32_t m_currentFrame;
    uint32_t m_imageIndex;

    // Helper methods
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

    void UpdateUniformBuffer(uint32_t currentImage);
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void RecreateSwapchain();

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};

} // namespace Renderer
} // namespace Aqua