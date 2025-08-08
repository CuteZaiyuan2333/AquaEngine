#include "Application.h"
#include "../Renderer/VulkanContext.h"
#include "../Utils/Logger.h"
#include "../Utils/Config.h"
#include <chrono>
#include <iostream>
#include <thread>


namespace Aqua {
namespace Platform {

Application::Application() : m_window(nullptr), m_isRunning(false) {
    // Initialize logging system
    auto& logger = Utils::Logger::GetInstance();
    auto& config = Utils::Config::GetInstance();
    
    // Set log level from config
    logger.SetLogLevel(static_cast<Utils::LogLevel>(config.Get<int>("logging.level", 1)));
}

Application::~Application() { Cleanup(); }

bool Application::Initialize() {
  AQUA_LOG_INFO("Application", "Initializing Application...");

  auto& config = Utils::Config::GetInstance();

  // Create window with config values
  std::string title = config.Get<std::string>("renderer.window_title", "AquaEngine");
  int width = config.Get<int>("renderer.window_width", 1280);
  int height = config.Get<int>("renderer.window_height", 720);
  AquaEngine::Platform::WindowProperties windowProps(title, width, height);
  m_window = std::make_shared<AquaEngine::Platform::Window>(windowProps);

  if (!m_window) {
    AQUA_LOG_ERROR("Application", "Failed to create window");
    return false;
  }

  // Set window close callback
  m_window->SetEventCallback([this]() { m_isRunning = false; });

  // Create Vulkan context
  m_vulkanContext = std::make_shared<Renderer::VulkanContext>();

  // Initialize Vulkan context
  if (!m_vulkanContext->Initialize(m_window->GetNativeWindow(),
                                   m_window->GetWidth(),
                                   m_window->GetHeight())) {
    AQUA_LOG_ERROR("Application", "Failed to initialize Vulkan context!");
    return false;
  }

  AQUA_LOG_INFO("Application", "Application initialized successfully");
  return true;
}

void Application::Run() {
  m_isRunning = true;

  while (m_isRunning && !m_window->ShouldClose()) {
    m_window->OnUpdate();
    Update();
    Render();
  }

  Cleanup();
}

void Application::Update() {
  // Basic update logic
}

void Application::Render() {
  // Basic render logic
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