#pragma once

#include "VulkanContext.h"
#include <string>
#include <vulkan/vulkan.h>

namespace AquaEngine {

class Texture {
public:
  Texture();
  ~Texture();

  // Static method to set VulkanContext for all texture operations
  static void SetVulkanContext(Aqua::Renderer::VulkanContext *context);

  // Basic texture loading functionality
  bool LoadFromFile(const std::string &filePath);
  void Cleanup();

  // Getter methods
  VkImage GetImage() const { return m_image; }
  VkImageView GetImageView() const { return m_imageView; }
  VkSampler GetSampler() const { return m_sampler; }
  uint32_t GetWidth() const { return m_width; }
  uint32_t GetHeight() const { return m_height; }

private:
  VkImage m_image = VK_NULL_HANDLE;
  VkDeviceMemory m_imageMemory = VK_NULL_HANDLE;
  VkImageView m_imageView = VK_NULL_HANDLE;
  VkSampler m_sampler = VK_NULL_HANDLE;

  uint32_t m_width = 0;
  uint32_t m_height = 0;
  uint32_t m_channels = 0;

  // Helper methods
  bool CreateVulkanTexture(unsigned char *data, uint32_t width,
                           uint32_t height);
  bool CreateImage(uint32_t width, uint32_t height, VkFormat format,
                   VkImageTiling tiling, VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkImage &image,
                   VkDeviceMemory &imageMemory);
  bool CreateImageView(VkImage image, VkFormat format, VkImageView &imageView);
  bool CreateSampler(VkSampler &sampler);
  void TransitionImageLayout(VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout);
  void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                         uint32_t height);

  // Vulkan utility methods
  uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
  VkCommandBuffer BeginSingleTimeCommands();
  void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
};
} // namespace AquaEngine