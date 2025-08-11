#pragma once

#include "Renderer.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <chrono>

#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

namespace AquaVisual {

// Forward declarations
class Camera;
class Mesh;
class Texture;

struct QueueFamilyIndices {
  uint32_t graphicsFamily = UINT32_MAX;
  uint32_t presentFamily = UINT32_MAX;

  bool IsComplete() {
    return graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX;
  }
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class VulkanRenderer : public Renderer {
public:
  VulkanRenderer();
  virtual ~VulkanRenderer() override;

  // Renderer interface implementation
  bool Initialize() override;
  void Shutdown() override;
  bool BeginFrame() override;
  void EndFrame() override;
  void SetCamera(const Camera &camera) override;
  void RenderMesh(const Mesh &mesh, const Texture *texture = nullptr) override;
  void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f,
             float a = 1.0f) override;
  bool ShouldClose() const override;
  void PollEvents() override;
  void GetWindowSize(uint32_t &width, uint32_t &height) const override;
  void SetFrameRateLimit(FrameRateMode mode) override;

  // Configuration methods
  void SetConfig(const RendererConfig &config);

  // Internal initialization method
  bool Initialize(void *windowHandle);

  // Vulkan specific methods
  QueueFamilyIndices FindQueueFamilies(void *device);
  bool IsDeviceSuitable(void *device);
  bool CheckDeviceExtensionSupport(void *device);
  SwapChainSupportDetails QuerySwapChainSupport(void *device);
  bool CreateLogicalDevice();
  bool CreateSwapChain();
  uint32_t
  ChooseSwapSurfaceFormat(const std::vector<uint32_t> &availableFormats);
  uint32_t
  ChooseSwapPresentMode(const std::vector<uint32_t> &availablePresentModes);
  void ChooseSwapExtent(const void *capabilities);
  bool CreateImageViews();
  bool CreateRenderPass();
  bool CreateGraphicsPipeline();
  void CleanupSwapChain();

  // Depth buffer methods
  bool CreateDepthResources();
  uint32_t FindDepthFormat();
  uint32_t FindSupportedFormat(const std::vector<uint32_t> &candidates,
                               uint32_t tiling, uint32_t features);
  bool HasStencilComponent(uint32_t format);
  bool CreateImage(uint32_t width, uint32_t height, uint32_t format,
                   uint32_t tiling, uint32_t usage, uint32_t properties,
                   void *&image, void *&imageMemory);
  void *CreateImageView(void *image, uint32_t format, uint32_t aspectFlags);
  uint32_t FindMemoryType(uint32_t typeFilter, uint32_t properties);

  // Uniform buffer methods
  bool CreateUniformBuffers();
  bool CreateDescriptorSetLayout();
  bool CreateDescriptorPool();
  bool CreateDescriptorSets();
  void UpdateUniformBuffer(uint32_t currentImage);
  bool CreateBuffer(uint64_t size, uint32_t usage, uint32_t properties,
                    void *&buffer, void *&bufferMemory);

  // Texture methods
  bool CreateTextureImage();
  bool CreateTextureImageView();
  bool CreateTextureSampler();
  void TransitionImageLayout(void *image, uint32_t format, uint32_t oldLayout, uint32_t newLayout);
  void CopyBufferToImage(void *buffer, void *image, uint32_t width, uint32_t height);

private:
  // Internal methods
  bool CreateVulkanWindow();
  bool InitializeVulkan();
  bool CreateInstance();
  bool CreateSurface();
  bool PickPhysicalDevice();
  bool CreateFramebuffers();
  bool CreateCommandPool();
  bool CreateCommandBuffers();
  bool CreateSyncObjects();

  // Basic member variables
  std::unique_ptr<class Window> m_window;
  void *m_instance = nullptr;
  void *m_debugMessenger = nullptr;
  void *m_surface = nullptr;
  void *m_physicalDevice = nullptr;
  void *m_device = nullptr;
  void *m_graphicsQueue = nullptr;
  void *m_presentQueue = nullptr;
  void *m_swapChain = nullptr;
  void *m_renderPass = nullptr;

  std::vector<void *> m_swapChainImages;
  std::vector<void *> m_swapChainImageViews;
  std::vector<void *> m_swapChainFramebuffers;

  uint32_t m_swapChainImageFormat = 0;
  struct SwapChainExtent {
    uint32_t width, height;
  } m_swapChainExtent = {0, 0};

  // Depth buffer
  void *m_depthImage;
  void *m_depthImageMemory;
  void *m_depthImageView;
  uint32_t m_depthFormat;

  // Render pipeline
  void *m_pipelineLayout = nullptr;
  void *m_graphicsPipeline = nullptr;

  // Command buffers
  void *m_commandPool = nullptr;
  std::vector<void *> m_commandBuffers;

  // Synchronization objects
  static const int MAX_FRAMES_IN_FLIGHT = 2;
  std::vector<void *> m_imageAvailableSemaphores;
  std::vector<void *> m_renderFinishedSemaphores;
  std::vector<void *> m_inFlightFences;
  uint32_t m_currentFrame = 0;
  uint32_t m_currentImageIndex = 0;

  // Animation time tracking
  float m_animationTime = 0.0f;

  // Clear color state
  float m_clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};

  // Camera matrices
  struct CameraUBO {
    float viewMatrix[16];
    float projectionMatrix[16];
  };

  // Current camera data
  CameraUBO m_currentCameraUBO;

  // Uniform buffers for camera matrices
  std::vector<void *> m_uniformBuffers;
  std::vector<void *> m_uniformBuffersMemory;
  std::vector<void *> m_uniformBuffersMapped;

  // Descriptor sets for uniform buffers
  void *m_descriptorSetLayout = nullptr;
  void *m_descriptorPool = nullptr;
  std::vector<void *> m_descriptorSets;

  // Texture and sampler
  void *m_textureImage = nullptr;
  void *m_textureImageMemory = nullptr;
  void *m_textureImageView = nullptr;
  void *m_textureSampler = nullptr;

  // Helper methods
  std::vector<char> ReadFile(const std::string &filename);
  VkShaderModule CreateShaderModule(const std::vector<char> &code);

  // Window event handlers
  void OnWindowResize(int width, int height);

  // Configuration
  RendererConfig m_config;

  // Frame rate limiting
  FrameRateMode m_frameRateMode = FrameRateMode::FPS_60;
  std::chrono::high_resolution_clock::time_point m_lastFrameTime;
  std::chrono::duration<double> m_targetFrameTime;
  
  // Frame rate control methods
  void WaitForFrameRate();

  // Validation layers and extensions
  std::vector<const char *> m_validationLayers = {
      "VK_LAYER_KHRONOS_validation"};
  std::vector<const char *> m_deviceExtensions = {"VK_KHR_swapchain"};
  bool m_enableValidationLayers = true;
};

} // namespace AquaVisual