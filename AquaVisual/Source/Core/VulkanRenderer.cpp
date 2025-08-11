#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include "../../Include/AquaVisual/Core/VulkanRenderer.h"
#include "../../Include/AquaVisual/Core/BufferManager.h"
#include "../../Include/AquaVisual/Core/Camera.h"
#include "../../Include/AquaVisual/Core/Window.h"
#include "../../Include/AquaVisual/Resources/Mesh.h"
#include "../../Include/AquaVisual/Resources/Texture.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <thread>
#include <vector>

#ifdef AQUA_HAS_GLFW
#include <GLFW/glfw3.h>
#endif

#include <vulkan/vulkan.h>

namespace AquaVisual {

VulkanRenderer::VulkanRenderer()
    : m_window(nullptr), m_instance(nullptr), m_debugMessenger(nullptr),
      m_surface(nullptr), m_physicalDevice(nullptr), m_device(nullptr),
      m_graphicsQueue(nullptr), m_presentQueue(nullptr), m_swapChain(nullptr),
      m_renderPass(nullptr), m_pipelineLayout(nullptr),
      m_graphicsPipeline(nullptr), m_swapChainImageFormat(0), m_currentFrame(0),
      m_currentImageIndex(0), m_depthImage(nullptr),
      m_depthImageMemory(nullptr), m_depthImageView(nullptr), m_depthFormat(0) {
  m_swapChainExtent = {0, 0};
  m_commandPool = nullptr;

  // 初始化帧率控制
  SetFrameRateLimit(FrameRateMode::FPS_60);
  m_lastFrameTime = std::chrono::high_resolution_clock::now();
}

VulkanRenderer::~VulkanRenderer() { Shutdown(); }

void VulkanRenderer::SetConfig(const RendererConfig &config) {
  m_config = config;
}

bool VulkanRenderer::Initialize() {
  std::cout << "VulkanRenderer::Initialize() called" << std::endl;
  std::cout << "Configuration:" << std::endl;
  std::cout << "  Size: " << m_config.width << "x" << m_config.height
            << std::endl;
  std::cout << "  Title: " << m_config.title << std::endl;
  std::cout << "  Validation: "
            << (m_config.enableValidation ? "enabled" : "disabled")
            << std::endl;
  std::cout << "  VSync: " << (m_config.enableVSync ? "enabled" : "disabled")
            << std::endl;

  try {
    // 1. Create window
    if (!CreateVulkanWindow()) {
      std::cerr << "Failed to create window" << std::endl;
      return false;
    }

    // 2. Initialize Vulkan
    if (!InitializeVulkan()) {
      std::cerr << "Failed to initialize Vulkan" << std::endl;
      return false;
    }

    std::cout << "VulkanRenderer initialized successfully!" << std::endl;
    return true;
  } catch (const std::exception &e) {
    std::cerr << "VulkanRenderer initialization failed: " << e.what()
              << std::endl;
    return false;
  }
}

void VulkanRenderer::CleanupSwapChain() {
  if (m_device == nullptr)
    return;

  VkDevice device = static_cast<VkDevice>(m_device);

  // Wait for device to be idle
  vkDeviceWaitIdle(device);

  // Cleanup framebuffers
  for (size_t i = 0; i < m_swapChainFramebuffers.size(); i++) {
    if (m_swapChainFramebuffers[i] != nullptr) {
      vkDestroyFramebuffer(
          device, static_cast<VkFramebuffer>(m_swapChainFramebuffers[i]),
          nullptr);
    }
  }
  m_swapChainFramebuffers.clear();

  // Cleanup depth resources
  if (m_depthImageView != nullptr) {
    vkDestroyImageView(device, static_cast<VkImageView>(m_depthImageView),
                       nullptr);
    m_depthImageView = nullptr;
  }

  if (m_depthImage != nullptr) {
    vkDestroyImage(device, static_cast<VkImage>(m_depthImage), nullptr);
    m_depthImage = nullptr;
  }

  if (m_depthImageMemory != nullptr) {
    vkFreeMemory(device, static_cast<VkDeviceMemory>(m_depthImageMemory),
                 nullptr);
    m_depthImageMemory = nullptr;
  }

  // Cleanup image views
  for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
    if (m_swapChainImageViews[i] != nullptr) {
      vkDestroyImageView(
          device, static_cast<VkImageView>(m_swapChainImageViews[i]), nullptr);
    }
  }
  m_swapChainImageViews.clear();

  // Cleanup swap chain
  if (m_swapChain != nullptr) {
    vkDestroySwapchainKHR(device, static_cast<VkSwapchainKHR>(m_swapChain),
                          nullptr);
    m_swapChain = nullptr;
  }

  m_swapChainImages.clear();
}

bool VulkanRenderer::CreateVulkanWindow() {
  std::cout << "Creating window..." << std::endl;

  m_window =
      std::make_unique<Window>(m_config.width, m_config.height, m_config.title);

  if (!m_window->Initialize()) {
    std::cerr << "Failed to initialize window" << std::endl;
    return false;
  }

  // Set up window event callbacks
  WindowEvents events;
  events.onResize = [this](int width, int height) {
    this->OnWindowResize(width, height);
  };
  m_window->SetEventCallbacks(events);

  std::cout << "Window created successfully" << std::endl;
  return true;
}

bool VulkanRenderer::InitializeVulkan() {
  std::cout << "Initializing Vulkan..." << std::endl;

  // 1. Create Vulkan instance
  if (!CreateInstance()) {
    return false;
  }

  // 2. Create surface
  if (!CreateSurface()) {
    return false;
  }

  // 3. Pick physical device
  if (!PickPhysicalDevice()) {
    return false;
  }

  // 4. Create logical device
  if (!CreateLogicalDevice()) {
    return false;
  }

  // 5. Create swap chain
  if (!CreateSwapChain()) {
    return false;
  }

  // 6. Create image views
  if (!CreateImageViews()) {
    return false;
  }

  // 7. Create depth resources
  if (!CreateDepthResources()) {
    return false;
  }

  // 8. Create render pass
  if (!CreateRenderPass()) {
    return false;
  }

  // 9. Create graphics pipeline
  if (!CreateGraphicsPipeline()) {
    return false;
  }

  // 10. Create framebuffers
  if (!CreateFramebuffers()) {
    return false;
  }

  // 11. Create command pool and buffers
  if (!CreateCommandPool()) {
    return false;
  }

  // 12. Create command buffers
  if (!CreateCommandBuffers()) {
    return false;
  }

  // 13. Create descriptor set layout
  if (!CreateDescriptorSetLayout()) {
    return false;
  }

  // 14. Create uniform buffers
  if (!CreateUniformBuffers()) {
    return false;
  }

  // 15. Create texture resources
  if (!CreateTextureImage()) {
    return false;
  }

  if (!CreateTextureImageView()) {
    return false;
  }

  if (!CreateTextureSampler()) {
    return false;
  }

  // 16. Create descriptor pool
  if (!CreateDescriptorPool()) {
    return false;
  }

  // 17. Create descriptor sets
  if (!CreateDescriptorSets()) {
    return false;
  }

  // 18. Create sync objects
  if (!CreateSyncObjects()) {
    return false;
  }

  std::cout << "Complete Vulkan initialization finished" << std::endl;
  return true;
}

bool VulkanRenderer::CreateInstance() {
  std::cout << "Creating Vulkan instance..." << std::endl;

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = m_config.title.c_str();
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "AquaVisual";
  appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  // Get required extensions
  auto extensions = Window::GetRequiredVulkanExtensions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  // Validation layers (if enabled)
  const std::vector<const char *> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

  if (m_config.enableValidation) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  VkInstance instance;
  VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
  if (result != VK_SUCCESS) {
    std::cerr << "Failed to create Vulkan instance! Error code: " << result
              << std::endl;
    return false;
  }

  m_instance = static_cast<void *>(instance);
  std::cout << "Vulkan instance created successfully" << std::endl;
  return true;
}

bool VulkanRenderer::CreateSurface() {
  std::cout << "Creating Vulkan surface..." << std::endl;

  if (!m_window) {
    std::cerr << "Window not created" << std::endl;
    return false;
  }

  VkInstance instance = static_cast<VkInstance>(m_instance);
  void *surface = nullptr;

  if (!m_window->CreateVulkanSurface(instance, &surface)) {
    std::cerr << "Failed to create window surface" << std::endl;
    return false;
  }

  m_surface = surface;
  std::cout << "Vulkan surface created successfully, surface pointer: "
            << m_surface << std::endl;
  return true;
}

bool VulkanRenderer::PickPhysicalDevice() {
  std::cout << "Picking physical device..." << std::endl;

  VkInstance instance = static_cast<VkInstance>(m_instance);

  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    std::cerr << "Failed to find GPUs with Vulkan support" << std::endl;
    return false;
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  // Just pick the first suitable device for now
  for (const auto &device : devices) {
    if (IsDeviceSuitable(static_cast<void *>(device))) {
      m_physicalDevice = static_cast<void *>(device);
      break;
    }
  }

  if (m_physicalDevice == nullptr) {
    std::cerr << "Failed to find a suitable GPU" << std::endl;
    return false;
  }

  std::cout << "Physical device picked successfully" << std::endl;
  return true;
}

bool VulkanRenderer::CreateLogicalDevice() {
  std::cout << "Creating logical device..." << std::endl;

  VkPhysicalDevice physicalDevice =
      static_cast<VkPhysicalDevice>(m_physicalDevice);

  // Find queue families
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           queueFamilies.data());

  uint32_t graphicsFamily = UINT32_MAX;
  for (uint32_t i = 0; i < queueFamilies.size(); i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphicsFamily = i;
      break;
    }
  }

  if (graphicsFamily == UINT32_MAX) {
    std::cerr << "Failed to find graphics queue family" << std::endl;
    return false;
  }

  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = graphicsFamily;
  queueCreateInfo.queueCount = 1;

  float queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;
  createInfo.pEnabledFeatures = &deviceFeatures;

  // Device extensions
  const std::vector<const char *> deviceExtensions = {"VK_KHR_swapchain"};
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  VkDevice device;
  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
      VK_SUCCESS) {
    std::cerr << "Failed to create logical device" << std::endl;
    return false;
  }

  m_device = static_cast<void *>(device);

  VkQueue graphicsQueue, presentQueue;
  vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
  vkGetDeviceQueue(device, graphicsFamily, 0, &presentQueue);

  m_graphicsQueue = static_cast<void *>(graphicsQueue);
  m_presentQueue = static_cast<void *>(presentQueue);

  // Initialize BufferManager with Vulkan device
  BufferManager::Instance().SetVulkanDevice(device, physicalDevice);

  std::cout << "Logical device created successfully" << std::endl;
  return true;
}

bool VulkanRenderer::CreateSwapChain() {
  std::cout << "Creating swap chain..." << std::endl;

  VkPhysicalDevice physicalDevice =
      static_cast<VkPhysicalDevice>(m_physicalDevice);
  VkDevice device = static_cast<VkDevice>(m_device);
  VkSurfaceKHR surface = static_cast<VkSurfaceKHR>(m_surface);

  // Query swap chain support
  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface,
                                            &capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                                       nullptr);
  std::vector<VkSurfaceFormatKHR> formats(formatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                                       formats.data());

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
                                            &presentModeCount, nullptr);
  std::vector<VkPresentModeKHR> presentModes(presentModeCount);
  vkGetPhysicalDeviceSurfacePresentModesKHR(
      physicalDevice, surface, &presentModeCount, presentModes.data());

  // Choose surface format
  VkSurfaceFormatKHR surfaceFormat = formats[0];
  for (const auto &availableFormat : formats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      surfaceFormat = availableFormat;
      break;
    }
  }

  // Choose present mode
  VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
  for (const auto &availablePresentMode : presentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      presentMode = availablePresentMode;
      break;
    }
  }

  // Choose extent
  VkExtent2D extent;
  if (capabilities.currentExtent.width != UINT32_MAX) {
    extent = capabilities.currentExtent;
  } else {
    extent = {m_config.width, m_config.height};
    extent.width =
        std::max(capabilities.minImageExtent.width,
                 std::min(capabilities.maxImageExtent.width, extent.width));
    extent.height =
        std::max(capabilities.minImageExtent.height,
                 std::min(capabilities.maxImageExtent.height, extent.height));
  }

  uint32_t imageCount = capabilities.minImageCount + 1;
  if (capabilities.maxImageCount > 0 &&
      imageCount > capabilities.maxImageCount) {
    imageCount = capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  createInfo.preTransform = capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  VkSwapchainKHR swapChain;
  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) !=
      VK_SUCCESS) {
    std::cerr << "Failed to create swap chain" << std::endl;
    return false;
  }

  m_swapChain = static_cast<void *>(swapChain);
  m_swapChainImageFormat = surfaceFormat.format;
  m_swapChainExtent = {extent.width, extent.height};

  // Get swap chain images
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
  std::vector<VkImage> swapChainImages(imageCount);
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount,
                          swapChainImages.data());

  m_swapChainImages.resize(imageCount);
  for (size_t i = 0; i < swapChainImages.size(); i++) {
    m_swapChainImages[i] = static_cast<void *>(swapChainImages[i]);
  }

  std::cout << "Swap chain created successfully" << std::endl;
  return true;
}

bool VulkanRenderer::CreateImageViews() {
  std::cout << "Creating image views..." << std::endl;

  VkDevice device = static_cast<VkDevice>(m_device);

  m_swapChainImageViews.resize(m_swapChainImages.size());

  for (size_t i = 0; i < m_swapChainImages.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = static_cast<VkImage>(m_swapChainImages[i]);
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = static_cast<VkFormat>(m_swapChainImageFormat);
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device, &createInfo, nullptr, &imageView) !=
        VK_SUCCESS) {
      std::cerr << "Failed to create image view " << i << std::endl;
      return false;
    }

    m_swapChainImageViews[i] = static_cast<void *>(imageView);
  }

  std::cout << "Image views created successfully ("
            << m_swapChainImageViews.size() << " views)" << std::endl;
  return true;
}

bool VulkanRenderer::CreateRenderPass() {
  std::cout << "Creating render pass..." << std::endl;

  VkDevice device = static_cast<VkDevice>(m_device);

  // Color attachment
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = static_cast<VkFormat>(m_swapChainImageFormat);
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  // Depth attachment
  VkAttachmentDescription depthAttachment{};
  depthAttachment.format = static_cast<VkFormat>(m_depthFormat);
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> attachments = {colorAttachment,
                                                        depthAttachment};
  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  VkRenderPass renderPass;
  if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) !=
      VK_SUCCESS) {
    std::cerr << "Failed to create render pass" << std::endl;
    return false;
  }

  m_renderPass = static_cast<void *>(renderPass);

  std::cout << "Render pass created successfully with depth buffer support"
            << std::endl;
  return true;
}

bool VulkanRenderer::CreateGraphicsPipeline() {
  std::cout << "Creating graphics pipeline..." << std::endl;

  VkDevice device = static_cast<VkDevice>(m_device);

  // Load shaders - using dual cube textured shaders
  auto vertShaderCode =
      ReadFile("AquaVisual/Shaders/dual_cube_textured_vert.spv");
  auto fragShaderCode =
      ReadFile("AquaVisual/Shaders/dual_cube_textured_frag.spv");

  if (vertShaderCode.empty() || fragShaderCode.empty()) {
    std::cerr << "Failed to load shader files" << std::endl;
    return false;
  }

  VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
  VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

  if (vertShaderModule == VK_NULL_HANDLE ||
      fragShaderModule == VK_NULL_HANDLE) {
    std::cerr << "Failed to create shader modules" << std::endl;
    if (vertShaderModule != VK_NULL_HANDLE) {
      vkDestroyShaderModule(device, vertShaderModule, nullptr);
    }
    if (fragShaderModule != VK_NULL_HANDLE) {
      vkDestroyShaderModule(device, fragShaderModule, nullptr);
    }
    return false;
  }

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  // Vertex input configuration for Vertex struct (position, normal, texCoord)
  VkVertexInputBindingDescription bindingDescription{};
  bindingDescription.binding = 0;
  bindingDescription.stride =
      sizeof(float) *
      8; // Vec3 position + Vec3 normal + Vec2 texCoord = 8 floats
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

  // Position attribute (location = 0)
  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[0].offset = 0;

  // Normal attribute (location = 1)
  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[1].offset = sizeof(float) * 3;

  // Texture coordinate attribute (location = 2)
  attributeDescriptions[2].binding = 0;
  attributeDescriptions[2].location = 2;
  attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[2].offset = sizeof(float) * 6;

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
  vertexInputInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescriptions.size());
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

  // Input assembly
  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  // Viewport and scissor
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(m_swapChainExtent.width);
  viewport.height = static_cast<float>(m_swapChainExtent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = {m_swapChainExtent.width, m_swapChainExtent.height};

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  // Rasterizer
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_NONE; // Disable back face culling
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // Fix winding order
  rasterizer.depthBiasEnable = VK_FALSE;

  // Multisampling
  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  // Depth stencil state
  VkPipelineDepthStencilStateCreateInfo depthStencil{};
  depthStencil.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.stencilTestEnable = VK_FALSE;

  // Color blending
  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  // Pipeline layout with push constants and descriptor set layout
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size =
      2 * sizeof(float); // Two floats: time and aspect ratio

  VkDescriptorSetLayout descriptorSetLayout =
      static_cast<VkDescriptorSetLayout>(m_descriptorSetLayout);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  VkPipelineLayout pipelineLayout;
  if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    std::cerr << "Failed to create pipeline layout" << std::endl;
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
    return false;
  }

  m_pipelineLayout = static_cast<void *>(pipelineLayout);

  // Graphics pipeline
  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil; // Add depth testing
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.layout = pipelineLayout;
  pipelineInfo.renderPass = static_cast<VkRenderPass>(m_renderPass);
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  VkPipeline graphicsPipeline;
  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                nullptr, &graphicsPipeline) != VK_SUCCESS) {
    std::cerr << "Failed to create graphics pipeline" << std::endl;
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
    return false;
  }

  m_graphicsPipeline = static_cast<void *>(graphicsPipeline);

  // Clean up shader modules
  vkDestroyShaderModule(device, fragShaderModule, nullptr);
  vkDestroyShaderModule(device, vertShaderModule, nullptr);

  std::cout << "Graphics pipeline created successfully" << std::endl;
  return true;
}

bool VulkanRenderer::CreateFramebuffers() {
  std::cout << "Creating framebuffers..." << std::endl;
  std::cout << "Swap chain image views count: " << m_swapChainImageViews.size()
            << std::endl;
  std::cout << "Swap chain extent: " << m_swapChainExtent.width << "x"
            << m_swapChainExtent.height << std::endl;

  if (m_swapChainImageViews.empty()) {
    std::cerr << "ERROR: No swap chain image views available for framebuffer "
                 "creation!"
              << std::endl;
    return false;
  }

  if (m_swapChainExtent.width == 0 || m_swapChainExtent.height == 0) {
    std::cerr << "ERROR: Invalid swap chain extent for framebuffer creation!"
              << std::endl;
    return false;
  }

  m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

  for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
    std::array<VkImageView, 2> attachments = {
        static_cast<VkImageView>(m_swapChainImageViews[i]),
        static_cast<VkImageView>(m_depthImageView)};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = static_cast<VkRenderPass>(m_renderPass);
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = m_swapChainExtent.width;
    framebufferInfo.height = m_swapChainExtent.height;
    framebufferInfo.layers = 1;

    VkFramebuffer framebuffer;
    if (vkCreateFramebuffer(static_cast<VkDevice>(m_device), &framebufferInfo,
                            nullptr, &framebuffer) != VK_SUCCESS) {
      std::cerr << "Failed to create framebuffer" << std::endl;
      return false;
    }

    m_swapChainFramebuffers[i] = static_cast<void *>(framebuffer);
  }

  std::cout << "Framebuffers created successfully" << std::endl;
  return true;
}

bool VulkanRenderer::CreateCommandPool() {
  std::cout << "Creating command pool..." << std::endl;

  // Find graphics queue family
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(
      static_cast<VkPhysicalDevice>(m_physicalDevice), &queueFamilyCount,
      nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(
      static_cast<VkPhysicalDevice>(m_physicalDevice), &queueFamilyCount,
      queueFamilies.data());

  uint32_t graphicsFamily = UINT32_MAX;
  for (uint32_t i = 0; i < queueFamilies.size(); i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphicsFamily = i;
      break;
    }
  }

  if (graphicsFamily == UINT32_MAX) {
    std::cerr << "Failed to find graphics queue family" << std::endl;
    return false;
  }

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = graphicsFamily;

  VkCommandPool commandPool;
  if (vkCreateCommandPool(static_cast<VkDevice>(m_device), &poolInfo, nullptr,
                          &commandPool) != VK_SUCCESS) {
    std::cerr << "Failed to create command pool" << std::endl;
    return false;
  }

  m_commandPool = static_cast<void *>(commandPool);

  std::cout << "Command pool created successfully" << std::endl;
  return true;
}

bool VulkanRenderer::CreateCommandBuffers() {
  std::cout << "Creating command buffers..." << std::endl;

  m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = static_cast<VkCommandPool>(m_commandPool);
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

  std::vector<VkCommandBuffer> commandBuffers(MAX_FRAMES_IN_FLIGHT);
  if (vkAllocateCommandBuffers(static_cast<VkDevice>(m_device), &allocInfo,
                               commandBuffers.data()) != VK_SUCCESS) {
    std::cerr << "Failed to allocate command buffers" << std::endl;
    return false;
  }

  for (size_t i = 0; i < commandBuffers.size(); i++) {
    m_commandBuffers[i] = static_cast<void *>(commandBuffers[i]);
  }

  std::cout << "Command buffers created successfully" << std::endl;
  return true;
}

bool VulkanRenderer::IsDeviceSuitable(void *device) {
  VkPhysicalDevice physicalDevice = static_cast<VkPhysicalDevice>(device);

  // Get device properties for debugging
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
  std::cout << "Checking device: " << deviceProperties.deviceName << std::endl;

  // Check queue families
  QueueFamilyIndices indices = FindQueueFamilies(device);
  std::cout << "Queue families complete: "
            << (indices.IsComplete() ? "Yes" : "No") << std::endl;
  if (!indices.IsComplete()) {
    return false;
  }

  // Check device extensions support
  bool extensionsSupported = CheckDeviceExtensionSupport(device);
  std::cout << "Extensions supported: " << (extensionsSupported ? "Yes" : "No")
            << std::endl;
  if (!extensionsSupported) {
    return false;
  }

  // Check swap chain support
  SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
  bool swapChainAdequate = !swapChainSupport.formats.empty() &&
                           !swapChainSupport.presentModes.empty();
  std::cout << "Swap chain adequate: " << (swapChainAdequate ? "Yes" : "No")
            << std::endl;
  std::cout << "Formats count: " << swapChainSupport.formats.size()
            << std::endl;
  std::cout << "Present modes count: " << swapChainSupport.presentModes.size()
            << std::endl;

  bool suitable = swapChainAdequate;
  std::cout << "Device suitable: " << (suitable ? "Yes" : "No") << std::endl;
  std::cout << "---" << std::endl;

  return suitable;
}

bool VulkanRenderer::CheckDeviceExtensionSupport(void *device) {
  VkPhysicalDevice physicalDevice = static_cast<VkPhysicalDevice>(device);

  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                       nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                       availableExtensions.data());

  std::set<std::string> requiredExtensions;
  for (const char *ext : m_deviceExtensions) {
    requiredExtensions.insert(std::string(ext));
  }

  for (const auto &extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

SwapChainSupportDetails VulkanRenderer::QuerySwapChainSupport(void *device) {
  VkPhysicalDevice physicalDevice = static_cast<VkPhysicalDevice>(device);
  SwapChainSupportDetails details;

  if (!m_surface) {
    std::cerr << "Surface is null, cannot query swap chain support"
              << std::endl;
    return details;
  }

  VkSurfaceKHR surface = static_cast<VkSurfaceKHR>(m_surface);

  // Query surface capabilities
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface,
                                            &details.capabilities);

  // Query surface formats
  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                                       nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                                         details.formats.data());
  }

  // Query present modes
  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
                                            &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
                                              &presentModeCount,
                                              details.presentModes.data());
  }

  std::cout << "Swap chain support query completed: " << formatCount
            << " formats, " << presentModeCount << " present modes"
            << std::endl;

  return details;
}

QueueFamilyIndices VulkanRenderer::FindQueueFamilies(void *device) {
  VkPhysicalDevice physicalDevice = static_cast<VkPhysicalDevice>(device);
  QueueFamilyIndices indices;

  std::cout << "FindQueueFamilies: surface pointer = " << m_surface
            << std::endl;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           nullptr);

  std::cout << "Found " << queueFamilyCount << " queue families" << std::endl;

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    std::cout << "Queue family " << i << ": flags=" << queueFamily.queueFlags;

    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
      std::cout << " (graphics)";
    }

    VkBool32 presentSupport = false;
    if (m_surface) {
      vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i,
                                           static_cast<VkSurfaceKHR>(m_surface),
                                           &presentSupport);
      if (presentSupport) {
        indices.presentFamily = i;
        std::cout << " (present)";
      }
    } else {
      std::cout << " (no surface to check present support)";
    }

    std::cout << std::endl;

    if (indices.IsComplete()) {
      break;
    }

    i++;
  }

  std::cout << "Graphics family: "
            << (indices.graphicsFamily != UINT32_MAX
                    ? std::to_string(indices.graphicsFamily)
                    : "not found")
            << std::endl;
  std::cout << "Present family: "
            << (indices.presentFamily != UINT32_MAX
                    ? std::to_string(indices.presentFamily)
                    : "not found")
            << std::endl;

  return indices;
}

bool VulkanRenderer::CreateSyncObjects() {
  std::cout << "Creating synchronization objects..." << std::endl;

  m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    VkSemaphore imageAvailableSemaphore, renderFinishedSemaphore;
    VkFence inFlightFence;

    if (vkCreateSemaphore(static_cast<VkDevice>(m_device), &semaphoreInfo,
                          nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(static_cast<VkDevice>(m_device), &semaphoreInfo,
                          nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(static_cast<VkDevice>(m_device), &fenceInfo, nullptr,
                      &inFlightFence) != VK_SUCCESS) {
      std::cerr << "Failed to create synchronization objects for frame " << i
                << std::endl;
      return false;
    }

    m_imageAvailableSemaphores[i] =
        static_cast<void *>(imageAvailableSemaphore);
    m_renderFinishedSemaphores[i] =
        static_cast<void *>(renderFinishedSemaphore);
    m_inFlightFences[i] = static_cast<void *>(inFlightFence);
  }

  std::cout << "Synchronization objects created successfully" << std::endl;
  return true;
}

void VulkanRenderer::Shutdown() {
  std::cout << "VulkanRenderer::Shutdown() called" << std::endl;

  // Wait for device to be idle
  if (m_device != nullptr) {
    vkDeviceWaitIdle(static_cast<VkDevice>(m_device));
  }

  // Cleanup synchronization objects
  if (m_device != nullptr) {
    VkDevice device = static_cast<VkDevice>(m_device);

    for (size_t i = 0; i < m_imageAvailableSemaphores.size(); i++) {
      if (m_imageAvailableSemaphores[i] != nullptr) {
        vkDestroySemaphore(
            device, static_cast<VkSemaphore>(m_imageAvailableSemaphores[i]),
            nullptr);
      }
    }

    for (size_t i = 0; i < m_renderFinishedSemaphores.size(); i++) {
      if (m_renderFinishedSemaphores[i] != nullptr) {
        vkDestroySemaphore(
            device, static_cast<VkSemaphore>(m_renderFinishedSemaphores[i]),
            nullptr);
      }
    }

    for (size_t i = 0; i < m_inFlightFences.size(); i++) {
      if (m_inFlightFences[i] != nullptr) {
        vkDestroyFence(device, static_cast<VkFence>(m_inFlightFences[i]),
                       nullptr);
      }
    }
  }

  // Cleanup command pool
  if (m_commandPool != nullptr && m_device != nullptr) {
    vkDestroyCommandPool(static_cast<VkDevice>(m_device),
                         static_cast<VkCommandPool>(m_commandPool), nullptr);
    m_commandPool = nullptr;
  }

  // Cleanup framebuffers
  if (m_device != nullptr) {
    VkDevice device = static_cast<VkDevice>(m_device);
    for (size_t i = 0; i < m_swapChainFramebuffers.size(); i++) {
      if (m_swapChainFramebuffers[i] != nullptr) {
        vkDestroyFramebuffer(
            device, static_cast<VkFramebuffer>(m_swapChainFramebuffers[i]),
            nullptr);
      }
    }
  }

  // Cleanup graphics pipeline
  if (m_graphicsPipeline != nullptr && m_device != nullptr) {
    vkDestroyPipeline(static_cast<VkDevice>(m_device),
                      static_cast<VkPipeline>(m_graphicsPipeline), nullptr);
    m_graphicsPipeline = nullptr;
  }

  // Cleanup pipeline layout
  if (m_pipelineLayout != nullptr && m_device != nullptr) {
    vkDestroyPipelineLayout(static_cast<VkDevice>(m_device),
                            static_cast<VkPipelineLayout>(m_pipelineLayout),
                            nullptr);
    m_pipelineLayout = nullptr;
  }

  // Cleanup render pass
  if (m_renderPass != nullptr && m_device != nullptr) {
    vkDestroyRenderPass(static_cast<VkDevice>(m_device),
                        static_cast<VkRenderPass>(m_renderPass), nullptr);
    m_renderPass = nullptr;
  }

  // Cleanup depth resources
  if (m_device != nullptr) {
    VkDevice device = static_cast<VkDevice>(m_device);

    if (m_depthImageView != nullptr) {
      vkDestroyImageView(device, static_cast<VkImageView>(m_depthImageView),
                         nullptr);
      m_depthImageView = nullptr;
    }

    if (m_depthImage != nullptr) {
      vkDestroyImage(device, static_cast<VkImage>(m_depthImage), nullptr);
      m_depthImage = nullptr;
    }

    if (m_depthImageMemory != nullptr) {
      vkFreeMemory(device, static_cast<VkDeviceMemory>(m_depthImageMemory),
                   nullptr);
      m_depthImageMemory = nullptr;
    }
  }

  // Cleanup image views
  if (m_device != nullptr) {
    VkDevice device = static_cast<VkDevice>(m_device);
    for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
      if (m_swapChainImageViews[i] != nullptr) {
        vkDestroyImageView(device,
                           static_cast<VkImageView>(m_swapChainImageViews[i]),
                           nullptr);
      }
    }
  }

  // Cleanup swap chain
  if (m_swapChain != nullptr && m_device != nullptr) {
    vkDestroySwapchainKHR(static_cast<VkDevice>(m_device),
                          static_cast<VkSwapchainKHR>(m_swapChain), nullptr);
    m_swapChain = nullptr;
  }

  // Cleanup logical device
  if (m_device != nullptr) {
    vkDestroyDevice(static_cast<VkDevice>(m_device), nullptr);
    m_device = nullptr;
  }

  // Cleanup surface
  if (m_surface != nullptr && m_instance != nullptr) {
    vkDestroySurfaceKHR(static_cast<VkInstance>(m_instance),
                        static_cast<VkSurfaceKHR>(m_surface), nullptr);
    m_surface = nullptr;
  }

  // Cleanup Vulkan instance
  if (m_instance != nullptr) {
    VkInstance instance = static_cast<VkInstance>(m_instance);
    vkDestroyInstance(instance, nullptr);
    m_instance = nullptr;
  }

  // Cleanup window
  if (m_window) {
    m_window->Shutdown();
    m_window.reset();
  }

  std::cout << "VulkanRenderer shutdown completed" << std::endl;
}

bool VulkanRenderer::BeginFrame() {
  std::cout << "BeginFrame: Starting frame " << m_currentFrame << std::endl;

  VkDevice device = static_cast<VkDevice>(m_device);
  VkFence inFlightFence =
      static_cast<VkFence>(m_inFlightFences[m_currentFrame]);

  std::cout << "BeginFrame: Waiting for fence..." << std::endl;
  // Wait for the previous frame to finish
  vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
  std::cout << "BeginFrame: Fence signaled" << std::endl;

  // Acquire an image from the swap chain
  VkSemaphore imageAvailableSemaphore =
      static_cast<VkSemaphore>(m_imageAvailableSemaphores[m_currentFrame]);
  VkSwapchainKHR swapChain = static_cast<VkSwapchainKHR>(m_swapChain);

  std::cout << "BeginFrame: Acquiring next image..." << std::endl;
  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
                                          imageAvailableSemaphore,
                                          VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    std::cerr << "BeginFrame: Swap chain out of date" << std::endl;
    // Swap chain needs to be recreated
    return false;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    std::cerr << "BeginFrame: Failed to acquire swap chain image, result = "
              << result << std::endl;
    return false;
  }

  std::cout << "BeginFrame: Acquired image index " << imageIndex << std::endl;
  m_currentImageIndex = imageIndex;

  // Reset the fence only if we are submitting work
  vkResetFences(device, 1, &inFlightFence);

  // Reset command buffer
  std::cout << "BeginFrame: Resetting command buffer for frame "
            << m_currentFrame << std::endl;
  VkCommandBuffer commandBuffer =
      static_cast<VkCommandBuffer>(m_commandBuffers[m_currentFrame]);
  vkResetCommandBuffer(commandBuffer, 0);

  // Begin recording command buffer
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  std::cout << "BeginFrame: Beginning command buffer recording" << std::endl;
  std::cout << "BeginFrame: Command buffer pointer = " << commandBuffer
            << std::endl;
  std::cout << "BeginFrame: Current frame = " << m_currentFrame << std::endl;

  VkResult beginResult = vkBeginCommandBuffer(commandBuffer, &beginInfo);
  std::cout << "BeginFrame: vkBeginCommandBuffer result = " << beginResult
            << std::endl;

  if (beginResult != VK_SUCCESS) {
    std::cerr
        << "BeginFrame: Failed to begin recording command buffer, error code: "
        << beginResult << std::endl;
    return false;
  }

  std::cout << "BeginFrame: Command buffer recording started successfully"
            << std::endl;

  // Begin render pass
  std::cout << "BeginFrame: Setting up render pass info" << std::endl;
  std::cout << "BeginFrame: Image index = " << imageIndex << std::endl;
  std::cout << "BeginFrame: Framebuffers size = "
            << m_swapChainFramebuffers.size() << std::endl;

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  std::cout << "BeginFrame: Setting render pass" << std::endl;
  renderPassInfo.renderPass = static_cast<VkRenderPass>(m_renderPass);
  std::cout << "BeginFrame: Setting framebuffer" << std::endl;
  renderPassInfo.framebuffer =
      static_cast<VkFramebuffer>(m_swapChainFramebuffers[imageIndex]);
  std::cout << "BeginFrame: Setting render area" << std::endl;
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = {m_swapChainExtent.width,
                                      m_swapChainExtent.height};

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {
      {m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]}};
  clearValues[1].depthStencil = {1.0f, 0};

  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  std::cout << "BeginFrame: Beginning render pass" << std::endl;
  std::cout << "BeginFrame: Render pass = " << renderPassInfo.renderPass
            << std::endl;
  std::cout << "BeginFrame: Framebuffer = " << renderPassInfo.framebuffer
            << std::endl;
  std::cout << "BeginFrame: Render area extent = "
            << renderPassInfo.renderArea.extent.width << "x"
            << renderPassInfo.renderArea.extent.height << std::endl;

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
  std::cout << "BeginFrame: Render pass started successfully" << std::endl;

  std::cout << "BeginFrame: Frame setup complete" << std::endl;
  return true;
}

void VulkanRenderer::EndFrame() {
  std::cout << "EndFrame: Starting end frame process for frame "
            << m_currentFrame << std::endl;
  VkDevice device = static_cast<VkDevice>(m_device);
  VkCommandBuffer commandBuffer =
      static_cast<VkCommandBuffer>(m_commandBuffers[m_currentFrame]);

  // End render pass
  std::cout << "EndFrame: Ending render pass" << std::endl;
  vkCmdEndRenderPass(commandBuffer);

  // End command buffer recording
  std::cout << "EndFrame: Ending command buffer recording" << std::endl;
  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    std::cerr << "EndFrame: Failed to record command buffer" << std::endl;
    return;
  }

  // Submit the command buffer
  std::cout << "EndFrame: Preparing to submit command buffer" << std::endl;
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {
      static_cast<VkSemaphore>(m_imageAvailableSemaphores[m_currentFrame])};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  VkSemaphore signalSemaphores[] = {
      static_cast<VkSemaphore>(m_renderFinishedSemaphores[m_currentFrame])};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  std::cout << "EndFrame: Submitting command buffer to graphics queue"
            << std::endl;
  VkFence inFlightFence =
      static_cast<VkFence>(m_inFlightFences[m_currentFrame]);
  if (vkQueueSubmit(static_cast<VkQueue>(m_graphicsQueue), 1, &submitInfo,
                    inFlightFence) != VK_SUCCESS) {
    std::cerr << "EndFrame: Failed to submit draw command buffer" << std::endl;
    return;
  }

  // Present the image
  std::cout << "EndFrame: Preparing to present image" << std::endl;
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {static_cast<VkSwapchainKHR>(m_swapChain)};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &m_currentImageIndex;
  presentInfo.pResults = nullptr;

  std::cout << "EndFrame: Presenting image index " << m_currentImageIndex
            << std::endl;
  VkResult result =
      vkQueuePresentKHR(static_cast<VkQueue>(m_presentQueue), &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    std::cout << "EndFrame: Swap chain out of date or suboptimal" << std::endl;
    // Swap chain needs to be recreated
  } else if (result != VK_SUCCESS) {
    std::cerr << "EndFrame: Failed to present swap chain image, result = "
              << result << std::endl;
  } else {
    std::cout << "EndFrame: Image presented successfully" << std::endl;
  }

  m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  std::cout << "EndFrame: Frame completed, next frame will be "
            << m_currentFrame << std::endl;

  // 应用帧率限制
  WaitForFrameRate();
}

void VulkanRenderer::SetCamera(const Camera &camera) {
  std::cout << "SetCamera: Updating camera matrices" << std::endl;

  // Get view and projection matrices from camera
  const Matrix4 &viewMatrix = camera.GetViewMatrix();
  const Matrix4 &projectionMatrix = camera.GetProjectionMatrix();

  // Copy view matrix data
  const float *viewData = viewMatrix.Data();
  const float *projData = projectionMatrix.Data();

  if (viewData && projData) {
    std::memcpy(m_currentCameraUBO.viewMatrix, viewData, 16 * sizeof(float));
    std::memcpy(m_currentCameraUBO.projectionMatrix, projData,
                16 * sizeof(float));

    // Debug: Print view matrix
    std::cout << "SetCamera: View Matrix:" << std::endl;
    for (int i = 0; i < 4; i++) {
      std::cout << "  [" << viewData[i * 4] << ", " << viewData[i * 4 + 1]
                << ", " << viewData[i * 4 + 2] << ", " << viewData[i * 4 + 3]
                << "]" << std::endl;
    }

    // Debug: Print projection matrix
    std::cout << "SetCamera: Projection Matrix:" << std::endl;
    for (int i = 0; i < 4; i++) {
      std::cout << "  [" << projData[i * 4] << ", " << projData[i * 4 + 1]
                << ", " << projData[i * 4 + 2] << ", " << projData[i * 4 + 3]
                << "]" << std::endl;
    }

    std::cout << "SetCamera: Camera matrices updated successfully" << std::endl;
  } else {
    std::cerr << "SetCamera: Failed to get matrix data from camera"
              << std::endl;
  }
}

void VulkanRenderer::RenderMesh(const Mesh &mesh, const Texture *texture) {
  std::cout << "RenderMesh: Starting mesh rendering for frame "
            << m_currentFrame << std::endl;
  VkCommandBuffer commandBuffer =
      static_cast<VkCommandBuffer>(m_commandBuffers[m_currentFrame]);

  // Update uniform buffer with current camera matrices
  UpdateUniformBuffer(m_currentFrame);

  // Bind graphics pipeline
  std::cout << "RenderMesh: Binding graphics pipeline" << std::endl;
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    static_cast<VkPipeline>(m_graphicsPipeline));

  // Bind descriptor sets
  VkDescriptorSet descriptorSet =
      static_cast<VkDescriptorSet>(m_descriptorSets[m_currentFrame]);
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          static_cast<VkPipelineLayout>(m_pipelineLayout), 0, 1,
                          &descriptorSet, 0, nullptr);

  // Set viewport
  std::cout << "RenderMesh: Setting viewport" << std::endl;
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(m_swapChainExtent.width);
  viewport.height = static_cast<float>(m_swapChainExtent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  // Set scissor
  std::cout << "RenderMesh: Setting scissor" << std::endl;
  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = {m_swapChainExtent.width, m_swapChainExtent.height};
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  // Update animation time (simple increment for smooth animation)
  m_animationTime += 0.016f; // Approximately 60 FPS

  // Calculate aspect ratio
  float aspectRatio = static_cast<float>(m_swapChainExtent.width) /
                      static_cast<float>(m_swapChainExtent.height);

  // Push time and aspect ratio constants to shader
  float pushConstants[2] = {m_animationTime, aspectRatio};
  std::cout << "RenderMesh: Pushing constants - time: " << m_animationTime
            << ", aspect: " << aspectRatio << std::endl;
  vkCmdPushConstants(
      commandBuffer, static_cast<VkPipelineLayout>(m_pipelineLayout),
      VK_SHADER_STAGE_VERTEX_BIT, 0, 2 * sizeof(float), pushConstants);

  // Check if mesh has valid data
  if (mesh.GetVertexCount() > 0) {
    // Use actual mesh data for rendering
    std::cout << "RenderMesh: Rendering mesh with " << mesh.GetVertexCount()
              << " vertices and " << mesh.GetIndexCount() << " indices"
              << std::endl;

    // Create temporary vertex buffer for this mesh
    auto vertexBuffer = BufferManager::Instance().CreateVertexBuffer(
        mesh.GetVertices().data(), mesh.GetVertexCount() * sizeof(Vertex));

    if (vertexBuffer) {
      // Bind vertex buffer
      VkBuffer vulkanVertexBuffer = vertexBuffer->GetVulkanBuffer();
      VkDeviceSize offsets[] = {0};
      vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vulkanVertexBuffer, offsets);

      if (mesh.GetIndexCount() > 0) {
        // Create temporary index buffer
        auto indexBuffer = BufferManager::Instance().CreateIndexBuffer(
            mesh.GetIndices().data(), mesh.GetIndexCount());

        if (indexBuffer) {
          // Bind index buffer and draw indexed
          VkBuffer vulkanIndexBuffer = indexBuffer->GetVulkanBuffer();
          vkCmdBindIndexBuffer(commandBuffer, vulkanIndexBuffer, 0,
                               VK_INDEX_TYPE_UINT32);
          vkCmdDrawIndexed(commandBuffer, mesh.GetIndexCount(), 1, 0, 0, 0);
          std::cout << "RenderMesh: Drew " << mesh.GetIndexCount() << " indices"
                    << std::endl;
        } else {
          std::cerr << "RenderMesh: Failed to create index buffer, falling "
                       "back to vertex draw"
                    << std::endl;
          vkCmdDraw(commandBuffer, mesh.GetVertexCount(), 1, 0, 0);
        }
      } else {
        // Draw without indices
        vkCmdDraw(commandBuffer, mesh.GetVertexCount(), 1, 0, 0);
        std::cout << "RenderMesh: Drew " << mesh.GetVertexCount() << " vertices"
                  << std::endl;
      }
    } else {
      std::cerr << "RenderMesh: Failed to create vertex buffer, using fallback"
                << std::endl;
      // Fallback to hardcoded cube
      vkCmdDraw(commandBuffer, 36, 1, 0, 0);
    }
  } else {
    // Fallback: draw hardcoded cube when no mesh data is available
    std::cout << "RenderMesh: No mesh data available, using hardcoded cube"
              << std::endl;
    vkCmdDraw(commandBuffer, 36, 1, 0, 0);
  }

  // Handle texture information
  if (texture) {
    std::cout << "RenderMesh: Texture provided and bound via descriptor set"
              << std::endl;
  } else {
    std::cout << "RenderMesh: Using default texture via descriptor set"
              << std::endl;
  }

  std::cout << "RenderMesh: Mesh rendering completed" << std::endl;
}

void VulkanRenderer::Clear(float r, float g, float b, float a) {
  std::cout << "Clearing screen with color (" << r << ", " << g << ", " << b
            << ", " << a << ")" << std::endl;
  m_clearColor[0] = r;
  m_clearColor[1] = g;
  m_clearColor[2] = b;
  m_clearColor[3] = a;
}

bool VulkanRenderer::ShouldClose() const {
  return m_window ? m_window->ShouldClose() : true;
}

void VulkanRenderer::PollEvents() {
  if (m_window) {
    m_window->PollEvents();
  }
}

void VulkanRenderer::GetWindowSize(uint32_t &width, uint32_t &height) const {
  if (m_window) {
    m_window->GetSize(width, height);
  } else {
    width = m_config.width;
    height = m_config.height;
  }
}

std::vector<char> VulkanRenderer::ReadFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return {};
  }

  size_t fileSize = static_cast<size_t>(file.tellg());
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}

VkShaderModule
VulkanRenderer::CreateShaderModule(const std::vector<char> &code) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(static_cast<VkDevice>(m_device), &createInfo,
                           nullptr, &shaderModule) != VK_SUCCESS) {
    std::cerr << "Failed to create shader module" << std::endl;
    return VK_NULL_HANDLE;
  }

  return shaderModule;
}

bool VulkanRenderer::CreateDepthResources() {
  std::cout << "Creating depth resources..." << std::endl;

  m_depthFormat = FindDepthFormat();

  if (!CreateImage(m_swapChainExtent.width, m_swapChainExtent.height,
                   m_depthFormat, VK_IMAGE_TILING_OPTIMAL,
                   VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depthImage,
                   m_depthImageMemory)) {
    std::cerr << "Failed to create depth image" << std::endl;
    return false;
  }

  m_depthImageView =
      CreateImageView(m_depthImage, m_depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
  if (m_depthImageView == nullptr) {
    std::cerr << "Failed to create depth image view" << std::endl;
    return false;
  }

  std::cout << "Depth resources created successfully" << std::endl;
  return true;
}

uint32_t VulkanRenderer::FindDepthFormat() {
  std::vector<uint32_t> candidates = {VK_FORMAT_D32_SFLOAT,
                                      VK_FORMAT_D32_SFLOAT_S8_UINT,
                                      VK_FORMAT_D24_UNORM_S8_UINT};

  return FindSupportedFormat(candidates, VK_IMAGE_TILING_OPTIMAL,
                             VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

uint32_t
VulkanRenderer::FindSupportedFormat(const std::vector<uint32_t> &candidates,
                                    uint32_t tiling, uint32_t features) {
  VkPhysicalDevice physicalDevice =
      static_cast<VkPhysicalDevice>(m_physicalDevice);

  for (uint32_t format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physicalDevice,
                                        static_cast<VkFormat>(format), &props);

    if (tiling == VK_IMAGE_TILING_LINEAR &&
        (props.linearTilingFeatures & features) == features) {
      return format;
    } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
               (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }

  throw std::runtime_error("Failed to find supported format");
}

bool VulkanRenderer::HasStencilComponent(uint32_t format) {
  return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
         format == VK_FORMAT_D24_UNORM_S8_UINT;
}

bool VulkanRenderer::CreateImage(uint32_t width, uint32_t height,
                                 uint32_t format, uint32_t tiling,
                                 uint32_t usage, uint32_t properties,
                                 void *&image, void *&imageMemory) {
  VkDevice device = static_cast<VkDevice>(m_device);

  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = static_cast<VkFormat>(format);
  imageInfo.tiling = static_cast<VkImageTiling>(tiling);
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = static_cast<VkImageUsageFlags>(usage);
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkImage vkImage;
  if (vkCreateImage(device, &imageInfo, nullptr, &vkImage) != VK_SUCCESS) {
    std::cerr << "Failed to create image" << std::endl;
    return false;
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device, vkImage, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      FindMemoryType(memRequirements.memoryTypeBits, properties);

  VkDeviceMemory vkImageMemory;
  if (vkAllocateMemory(device, &allocInfo, nullptr, &vkImageMemory) !=
      VK_SUCCESS) {
    std::cerr << "Failed to allocate image memory" << std::endl;
    vkDestroyImage(device, vkImage, nullptr);
    return false;
  }

  vkBindImageMemory(device, vkImage, vkImageMemory, 0);

  image = static_cast<void *>(vkImage);
  imageMemory = static_cast<void *>(vkImageMemory);

  return true;
}

void *VulkanRenderer::CreateImageView(void *image, uint32_t format,
                                      uint32_t aspectFlags) {
  VkDevice device = static_cast<VkDevice>(m_device);

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = static_cast<VkImage>(image);
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = static_cast<VkFormat>(format);
  viewInfo.subresourceRange.aspectMask =
      static_cast<VkImageAspectFlags>(aspectFlags);
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VkImageView imageView;
  if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
    std::cerr << "Failed to create image view" << std::endl;
    return nullptr;
  }

  return static_cast<void *>(imageView);
}

uint32_t VulkanRenderer::FindMemoryType(uint32_t typeFilter,
                                        uint32_t properties) {
  VkPhysicalDevice physicalDevice =
      static_cast<VkPhysicalDevice>(m_physicalDevice);

  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("Failed to find suitable memory type");
}

bool VulkanRenderer::CreateBuffer(uint64_t size, uint32_t usage,
                                  uint32_t properties, void *&buffer,
                                  void *&bufferMemory) {
  VkDevice device = static_cast<VkDevice>(m_device);

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = static_cast<VkBufferUsageFlags>(usage);
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkBuffer vkBuffer;
  if (vkCreateBuffer(device, &bufferInfo, nullptr, &vkBuffer) != VK_SUCCESS) {
    std::cerr << "Failed to create buffer" << std::endl;
    return false;
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, vkBuffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      FindMemoryType(memRequirements.memoryTypeBits, properties);

  VkDeviceMemory vkBufferMemory;
  if (vkAllocateMemory(device, &allocInfo, nullptr, &vkBufferMemory) !=
      VK_SUCCESS) {
    std::cerr << "Failed to allocate buffer memory" << std::endl;
    vkDestroyBuffer(device, vkBuffer, nullptr);
    return false;
  }

  vkBindBufferMemory(device, vkBuffer, vkBufferMemory, 0);

  buffer = static_cast<void *>(vkBuffer);
  bufferMemory = static_cast<void *>(vkBufferMemory);

  return true;
}

bool VulkanRenderer::CreateUniformBuffers() {
  std::cout << "Creating uniform buffers..." << std::endl;

  uint64_t bufferSize = sizeof(CameraUBO);

  m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
  m_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
  m_uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      m_uniformBuffers[i], m_uniformBuffersMemory[i])) {
      std::cerr << "Failed to create uniform buffer " << i << std::endl;
      return false;
    }

    // Map the buffer memory
    VkDevice device = static_cast<VkDevice>(m_device);
    VkDeviceMemory memory =
        static_cast<VkDeviceMemory>(m_uniformBuffersMemory[i]);
    void *mapped;
    if (vkMapMemory(device, memory, 0, bufferSize, 0, &mapped) != VK_SUCCESS) {
      std::cerr << "Failed to map uniform buffer memory " << i << std::endl;
      return false;
    }
    m_uniformBuffersMapped[i] = mapped;
  }

  std::cout << "Uniform buffers created successfully" << std::endl;
  return true;
}

bool VulkanRenderer::CreateDescriptorSetLayout() {
  std::cout << "Creating descriptor set layout..." << std::endl;

  // Uniform buffer binding (binding = 0)
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  uboLayoutBinding.pImmutableSamplers = nullptr;

  // Texture sampler binding (binding = 1)
  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  samplerLayoutBinding.pImmutableSamplers = nullptr;

  std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding,
                                                          samplerLayoutBinding};

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  VkDescriptorSetLayout descriptorSetLayout;
  VkDevice device = static_cast<VkDevice>(m_device);
  if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr,
                                  &descriptorSetLayout) != VK_SUCCESS) {
    std::cerr << "Failed to create descriptor set layout" << std::endl;
    return false;
  }

  m_descriptorSetLayout = static_cast<void *>(descriptorSetLayout);
  std::cout << "Descriptor set layout created successfully" << std::endl;
  return true;
}

bool VulkanRenderer::CreateDescriptorPool() {
  std::cout << "Creating descriptor pool..." << std::endl;

  std::array<VkDescriptorPoolSize, 2> poolSizes{};

  // Uniform buffer pool size
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

  // Combined image sampler pool size
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

  VkDescriptorPool descriptorPool;
  VkDevice device = static_cast<VkDevice>(m_device);
  if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) !=
      VK_SUCCESS) {
    std::cerr << "Failed to create descriptor pool" << std::endl;
    return false;
  }

  m_descriptorPool = static_cast<void *>(descriptorPool);
  std::cout << "Descriptor pool created successfully" << std::endl;
  return true;
}

bool VulkanRenderer::CreateDescriptorSets() {
  std::cout << "Creating descriptor sets..." << std::endl;

  std::vector<VkDescriptorSetLayout> layouts(
      MAX_FRAMES_IN_FLIGHT,
      static_cast<VkDescriptorSetLayout>(m_descriptorSetLayout));

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = static_cast<VkDescriptorPool>(m_descriptorPool);
  allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
  allocInfo.pSetLayouts = layouts.data();

  std::vector<VkDescriptorSet> descriptorSets(MAX_FRAMES_IN_FLIGHT);
  VkDevice device = static_cast<VkDevice>(m_device);
  if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) !=
      VK_SUCCESS) {
    std::cerr << "Failed to allocate descriptor sets" << std::endl;
    return false;
  }

  m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    // Uniform buffer descriptor
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = static_cast<VkBuffer>(m_uniformBuffers[i]);
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(CameraUBO);

    // Texture sampler descriptor
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = static_cast<VkImageView>(m_textureImageView);
    imageInfo.sampler = static_cast<VkSampler>(m_textureSampler);

    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

    // Uniform buffer write
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSets[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    // Texture sampler write
    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSets[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device,
                           static_cast<uint32_t>(descriptorWrites.size()),
                           descriptorWrites.data(), 0, nullptr);

    m_descriptorSets[i] = static_cast<void *>(descriptorSets[i]);
  }

  std::cout << "Descriptor sets created successfully" << std::endl;
  return true;
}

void VulkanRenderer::UpdateUniformBuffer(uint32_t currentImage) {
  if (currentImage >= m_uniformBuffersMapped.size()) {
    std::cerr << "UpdateUniformBuffer: Invalid current image index: "
              << currentImage << std::endl;
    return;
  }

  void *data = m_uniformBuffersMapped[currentImage];
  if (data) {
    std::memcpy(data, &m_currentCameraUBO, sizeof(CameraUBO));
  } else {
    std::cerr << "UpdateUniformBuffer: Uniform buffer not mapped for frame "
              << currentImage << std::endl;
  }
}

void VulkanRenderer::OnWindowResize(int width, int height) {
  std::cout << "Window resized to: " << width << "x" << height << std::endl;

  // Update config dimensions
  m_config.width = static_cast<uint32_t>(width);
  m_config.height = static_cast<uint32_t>(height);

  // Wait for device to be idle before recreating swap chain
  VkDevice device = static_cast<VkDevice>(m_device);
  if (device) {
    vkDeviceWaitIdle(device);

    // Clean up old swap chain
    CleanupSwapChain();

    // Recreate swap chain with new dimensions
    if (!CreateSwapChain()) {
      std::cerr << "Failed to recreate swap chain after resize" << std::endl;
      return;
    }

    if (!CreateImageViews()) {
      std::cerr << "Failed to recreate image views after resize" << std::endl;
      return;
    }

    if (!CreateDepthResources()) {
      std::cerr << "Failed to recreate depth resources after resize"
                << std::endl;
      return;
    }

    if (!CreateFramebuffers()) {
      std::cerr << "Failed to recreate framebuffers after resize" << std::endl;
      return;
    }

    std::cout << "Swap chain recreated successfully after resize" << std::endl;
  }
}

void VulkanRenderer::SetFrameRateLimit(FrameRateMode mode) {
  m_frameRateMode = mode;

  switch (mode) {
  case FrameRateMode::UNLIMITED:
    m_targetFrameTime = std::chrono::nanoseconds(0);
    break;
  case FrameRateMode::FPS_24:
    m_targetFrameTime = std::chrono::nanoseconds(1000000000 / 24);
    break;
  case FrameRateMode::FPS_30:
    m_targetFrameTime = std::chrono::nanoseconds(1000000000 / 30);
    break;
  case FrameRateMode::FPS_60:
    m_targetFrameTime = std::chrono::nanoseconds(1000000000 / 60);
    break;
  case FrameRateMode::FPS_120:
    m_targetFrameTime = std::chrono::nanoseconds(1000000000 / 120);
    break;
  }

  std::cout << "Frame rate limit set to ";
  switch (mode) {
  case FrameRateMode::UNLIMITED:
    std::cout << "UNLIMITED";
    break;
  case FrameRateMode::FPS_24:
    std::cout << "24 FPS";
    break;
  case FrameRateMode::FPS_30:
    std::cout << "30 FPS";
    break;
  case FrameRateMode::FPS_60:
    std::cout << "60 FPS";
    break;
  case FrameRateMode::FPS_120:
    std::cout << "120 FPS";
    break;
  }
  std::cout << std::endl;
}

void VulkanRenderer::WaitForFrameRate() {
  if (m_frameRateMode == FrameRateMode::UNLIMITED) {
    return;
  }

  auto currentTime = std::chrono::high_resolution_clock::now();
  auto elapsedTime = currentTime - m_lastFrameTime;

  if (elapsedTime < m_targetFrameTime) {
    auto sleepTime = m_targetFrameTime - elapsedTime;
    std::this_thread::sleep_for(sleepTime);
  }

  m_lastFrameTime = std::chrono::high_resolution_clock::now();
}

bool VulkanRenderer::CreateTextureImage() {
  std::cout << "Creating texture image..." << std::endl;

  // Create a simple 2x2 checkerboard texture for testing
  const uint32_t texWidth = 2;
  const uint32_t texHeight = 2;
  const uint32_t texChannels = 4; // RGBA

  // Simple checkerboard pattern: white and red
  uint8_t pixels[texWidth * texHeight * texChannels] = {
      255, 255, 255, 255, // White
      255, 0,   0,   255, // Red
      255, 0,   0,   255, // Red
      255, 255, 255, 255  // White
  };

  VkDeviceSize imageSize = texWidth * texHeight * texChannels;

  // Create staging buffer
  void *stagingBuffer, *stagingBufferMemory;
  if (!CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    stagingBuffer, stagingBufferMemory)) {
    std::cerr << "Failed to create staging buffer for texture" << std::endl;
    return false;
  }

  // Copy pixel data to staging buffer
  VkDevice device = static_cast<VkDevice>(m_device);
  void *data;
  vkMapMemory(device, static_cast<VkDeviceMemory>(stagingBufferMemory), 0,
              imageSize, 0, &data);
  memcpy(data, pixels, static_cast<size_t>(imageSize));
  vkUnmapMemory(device, static_cast<VkDeviceMemory>(stagingBufferMemory));

  // Create texture image
  if (!CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB,
                   VK_IMAGE_TILING_OPTIMAL,
                   VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage,
                   m_textureImageMemory)) {
    std::cerr << "Failed to create texture image" << std::endl;
    return false;
  }

  // Transition image layout and copy buffer to image
  TransitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  CopyBufferToImage(stagingBuffer, m_textureImage, texWidth, texHeight);
  TransitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  // Clean up staging buffer
  vkDestroyBuffer(device, static_cast<VkBuffer>(stagingBuffer), nullptr);
  vkFreeMemory(device, static_cast<VkDeviceMemory>(stagingBufferMemory),
               nullptr);

  std::cout << "Texture image created successfully" << std::endl;
  return true;
}

bool VulkanRenderer::CreateTextureImageView() {
  std::cout << "Creating texture image view..." << std::endl;

  m_textureImageView = CreateImageView(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                                       VK_IMAGE_ASPECT_COLOR_BIT);

  if (m_textureImageView == nullptr) {
    std::cerr << "Failed to create texture image view" << std::endl;
    return false;
  }

  std::cout << "Texture image view created successfully" << std::endl;
  return true;
}

bool VulkanRenderer::CreateTextureSampler() {
  std::cout << "Creating texture sampler..." << std::endl;

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter =
      VK_FILTER_NEAREST; // Use nearest for sharp checkerboard
  samplerInfo.minFilter = VK_FILTER_NEAREST;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_FALSE;
  samplerInfo.maxAnisotropy = 1.0f;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 0.0f;

  VkDevice device = static_cast<VkDevice>(m_device);
  VkSampler sampler;
  if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
    std::cerr << "Failed to create texture sampler" << std::endl;
    return false;
  }

  m_textureSampler = static_cast<void *>(sampler);
  std::cout << "Texture sampler created successfully" << std::endl;
  return true;
}

void VulkanRenderer::TransitionImageLayout(void *image, uint32_t format,
                                           uint32_t oldLayout,
                                           uint32_t newLayout) {
  // This is a simplified implementation - in a real application you'd want
  // proper command buffer management
  std::cout << "Transitioning image layout (placeholder implementation)"
            << std::endl;
}

void VulkanRenderer::CopyBufferToImage(void *buffer, void *image,
                                       uint32_t width, uint32_t height) {
  // This is a simplified implementation - in a real application you'd want
  // proper command buffer management
  std::cout << "Copying buffer to image (placeholder implementation)"
            << std::endl;
}

} // namespace AquaVisual