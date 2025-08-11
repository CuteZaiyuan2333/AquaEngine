#include "AquaVisual/AquaVisual.h"
#include "AquaVisual/Core/Renderer.h"
#include <chrono>
#include <iostream>
#include <thread>

using namespace AquaVisual;

int main() {
  std::cout << "=== AquaVisual Frame Rate Test ===" << std::endl;

  // Initialize AquaVisual
  if (!Initialize()) {
    std::cerr << "Failed to initialize AquaVisual" << std::endl;
    return -1;
  }

  // Create renderer configuration
  RendererConfig config;
  config.width = 800;
  config.height = 600;
  config.title = "Frame Rate Test";
  config.enableValidation = false;

  // Create renderer
  auto renderer = Renderer::Create(config);
  if (!renderer) {
    std::cerr << "Failed to create renderer" << std::endl;
    Shutdown();
    return -1;
  }

  std::cout << "Renderer created successfully!" << std::endl;

  // Test different frame rate modes
  std::vector<FrameRateMode> modes = {
      FrameRateMode::UNLIMITED, FrameRateMode::FPS_30, FrameRateMode::FPS_60,
      FrameRateMode::FPS_120};

  std::vector<std::string> modeNames = {"UNLIMITED", "30 FPS", "60 FPS",
                                        "120 FPS"};

  for (size_t i = 0; i < modes.size(); ++i) {
    std::cout << "\n=== Testing " << modeNames.at(i) << " ===" << std::endl;

    // Set frame rate mode
    renderer->SetFrameRateLimit(modes.at(i));

    // Measure frame times for 2 seconds
    auto startTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;

    while (true) {
      auto currentTime = std::chrono::high_resolution_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
          currentTime - startTime);

      if (elapsed.count() >= 2000) { // 2 seconds
        break;
      }

      // Simulate frame rendering
      renderer->BeginFrame();
      renderer->EndFrame();

      frameCount++;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime);

    double actualFPS = (frameCount * 1000.0) / totalTime.count();
    std::cout << "Frames rendered: " << frameCount << std::endl;
    std::cout << "Time elapsed: " << totalTime.count() << " ms" << std::endl;
    std::cout << "Actual FPS: " << actualFPS << std::endl;

    // Brief pause between tests
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  std::cout << "\n=== Frame Rate Test Completed ===" << std::endl;

  // Cleanup
  Shutdown();
  return 0;
}