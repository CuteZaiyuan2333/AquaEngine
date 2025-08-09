#pragma once

#include "../../Include/AquaVisual/Core/Renderer.h"
#include <vector>
#include <memory>
#include <string>

namespace AquaVisual {

/**
 * @brief Vulkan渲染器实现 - MVP版本
 */
class VulkanRenderer : public Renderer {
public:
    VulkanRenderer();
    ~VulkanRenderer() override;
    
    void SetConfig(const RendererConfig& config);

    bool Initialize() override;
    void Shutdown() override;
    bool BeginFrame() override;
    void EndFrame() override;
    void SetCamera(const Camera& camera) override;
    void RenderMesh(const Mesh& mesh, const Texture* texture = nullptr) override;
    void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) override;
    bool ShouldClose() const override;
    void PollEvents() override;
    void GetWindowSize(uint32_t& width, uint32_t& height) const override;

private:
    struct QueueFamilyIndices {
        uint32_t graphicsFamily = UINT32_MAX;
        uint32_t presentFamily = UINT32_MAX;

        bool IsComplete() const {
            return graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX;
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    // 初始化方法
    bool CreateWindow();
    bool CreateInstance();
    bool SetupDebugMessenger();
    bool CreateSurface();
    bool PickPhysicalDevice();
    bool CreateLogicalDevice();
    bool CreateSwapChain();
    bool CreateImageViews();
    bool CreateRenderPass();
    bool CreateDescriptorSetLayout();
    bool CreateGraphicsPipeline();
    bool CreateFramebuffers();
    bool CreateCommandPool();
    bool CreateCommandBuffers();
    bool CreateSyncObjects();

    // 辅助方法
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    bool IsDeviceSuitable(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    std::vector<const char*> GetRequiredExtensions();
    bool CheckValidationLayerSupport();

    // 清理方法
    void CleanupSwapChain();
    
    // 着色器辅助方法
    std::vector<char> ReadFile(const std::string& filename);
    VkShaderModule CreateShaderModule(const std::vector<char>& code);

    // 配置
    RendererConfig m_config;

    // 窗口
    std::unique_ptr<class Window> m_window;

    // Vulkan核心对象 (使用void*以避免头文件依赖)
    void* m_instance = nullptr;
    void* m_debugMessenger = nullptr;
    void* m_surface = nullptr;
    void* m_physicalDevice = nullptr;
    void* m_device = nullptr;
    void* m_graphicsQueue = nullptr;
    void* m_presentQueue = nullptr;

    // 交换链
    void* m_swapChain = nullptr;
    std::vector<void*> m_swapChainImages;
    uint32_t m_swapChainImageFormat = 0;
    struct { uint32_t width, height; } m_swapChainExtent = {0, 0};
    std::vector<void*> m_swapChainImageViews;
    std::vector<void*> m_swapChainFramebuffers;

    // 渲染管线
    void* m_renderPass = nullptr;
    void* m_descriptorSetLayout = nullptr;
    void* m_pipelineLayout = nullptr;
    void* m_graphicsPipeline = nullptr;

    // 命令
    void* m_commandPool = nullptr;
    std::vector<void*> m_commandBuffers;

    // 同步对象
    std::vector<void*> m_imageAvailableSemaphores;
    std::vector<void*> m_renderFinishedSemaphores;
    std::vector<void*> m_inFlightFences;

    // 帧状态
    uint32_t m_currentFrame = 0;
    uint32_t m_currentImageIndex = 0;
    bool m_framebufferResized = false;
    
    // 常量
    static const int MAX_FRAMES_IN_FLIGHT = 2;
    
    // 设备扩展
    const std::vector<const char*> m_deviceExtensions = {
        "VK_KHR_swapchain"
    };
};

} // namespace AquaVisual