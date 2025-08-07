#include "Application.h"
#include "../Renderer/VulkanContext.h"
#include <iostream>

namespace Aqua {
namespace Platform {

Application::Application() 
    : m_window(nullptr), m_isRunning(false) {
}

Application::~Application() {
    Cleanup();
}

bool Application::Initialize() {
    std::cout << "Initializing Application..." << std::endl;
    
    // 创建窗口
    AquaEngine::Platform::WindowProperties windowProps("AquaEngine", 1280, 720);
    m_window = std::make_shared<AquaEngine::Platform::Window>(windowProps);
    
    // 设置窗口关闭回调
    m_window->SetEventCallback([this]() {
        m_isRunning = false;
    });
    
    // 创建Vulkan上下文
    m_vulkanContext = std::make_shared<Renderer::VulkanContext>();
    
    // 初始化Vulkan上下文
    if (!m_vulkanContext->Initialize(m_window->GetNativeWindow(), m_window->GetWidth(), m_window->GetHeight())) {
        std::cerr << "Failed to initialize Vulkan context!" << std::endl;
        return false;
    }
    
    return true;
}

void Application::Run() {
    if (!Initialize()) {
        std::cerr << "Failed to initialize application!" << std::endl;
        return;
    }
    
    m_isRunning = true;
    
    while (m_isRunning && !m_window->ShouldClose()) {
        m_window->OnUpdate();
        Update();
        Render();
    }
    
    Cleanup();
}

void Application::Update() {
    // 基础更新逻辑
}

void Application::Render() {
    // 基础渲染逻辑
}

void Application::Cleanup() {
    if (m_vulkanContext) {
        m_vulkanContext->Cleanup();
        m_vulkanContext.reset();
    }
    
    if (m_window) {
        m_window.reset();
    }
}

std::shared_ptr<Renderer::VulkanContext> Application::GetVulkanContext() const {
    return m_vulkanContext;
}

} // namespace Platform
} // namespace Aqua