#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <optional>

namespace Aqua {
namespace Renderer {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanContext {
public:
    VulkanContext();
    ~VulkanContext();

    // 初始化Vulkan
    bool Initialize(void* windowHandle, uint32_t width, uint32_t height);
    void Cleanup();

    // 获取Vulkan对象
    VkInstance GetInstance() const { return m_instance; }
    VkDevice GetDevice() const { return m_device; }
    VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
    VkSurfaceKHR GetSurface() const { return m_surface; }
    VkSwapchainKHR GetSwapchain() const { return m_swapchain; }
    VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }
    VkQueue GetPresentQueue() const { return m_presentQueue; }

    // 获取交换链信息
    VkFormat GetSwapchainImageFormat() const { return m_swapchainImageFormat; }
    VkExtent2D GetSwapchainExtent() const { return m_swapchainExtent; }
    const std::vector<VkImage>& GetSwapchainImages() const { return m_swapchainImages; }
    const std::vector<VkImageView>& GetSwapchainImageViews() const { return m_swapchainImageViews; }

private:
    // Vulkan对象
    VkInstance m_instance = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    
    // 队列
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    
    // 交换链相关
    VkFormat m_swapchainImageFormat;
    VkExtent2D m_swapchainExtent;
    std::vector<VkImage> m_swapchainImages;
    std::vector<VkImageView> m_swapchainImageViews;

    // 调试相关
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

    // 私有方法
    bool CreateInstance();
    bool SetupDebugMessenger();
    bool CreateSurface(void* windowHandle);
    bool PickPhysicalDevice();
    bool CreateLogicalDevice();
    bool CreateSwapchain(uint32_t width, uint32_t height);
    bool CreateImageViews();

    // 辅助方法
    bool CheckValidationLayerSupport();
    std::vector<const char*> GetRequiredExtensions();
    bool IsDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

    // 常量
    static const std::vector<const char*> s_validationLayers;
    static const std::vector<const char*> s_deviceExtensions;

#ifdef AQUA_DEBUG
    static const bool s_enableValidationLayers = true;
#else
    static const bool s_enableValidationLayers = false;
#endif
};

} // namespace Renderer
} // namespace Aqua