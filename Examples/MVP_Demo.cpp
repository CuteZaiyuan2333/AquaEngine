#include <AquaVisual/AquaVisualMVP.h>
#include <chrono>
#include <iostream>

using namespace AquaVisual;

int main() {
  std::cout << "=== AquaVisual MVP Demo ===" << std::endl;
  std::cout << "Version: " << AQUAVISUAL_MVP_VERSION << std::endl;

  // 1. 创建渲染器配置
  RendererConfig config;
  config.width = 800;
  config.height = 600;
  config.title = "AquaVisual MVP Demo";
  config.enableValidation = true;
  config.enableVSync = true;

  // 2. 创建渲染器
  auto renderer = Renderer::Create(config);
  if (!renderer) {
    std::cerr << "Failed to create renderer!" << std::endl;
    return -1;
  }

  std::cout << "Renderer created successfully!" << std::endl;

  // 3. 创建相机
  Camera camera;
  camera.SetPosition(Vec3(0.0f, 0.0f, 3.0f));
  camera.SetTarget(Vec3(0.0f, 0.0f, 0.0f));
  camera.SetFOV(45.0f);
  camera.SetAspectRatio(800.0f / 600.0f);

  renderer->SetCamera(camera);
  std::cout << "Camera configured!" << std::endl;

  // 4. 创建几何体
  auto triangle = Primitives::CreateTriangle(1.0f);
  auto cube = Primitives::CreateCube(1.0f);
  auto sphere = Primitives::CreateSphere(1.0f, 16);

  std::cout << "Meshes created:" << std::endl;
  std::cout << "  Triangle: " << triangle->GetVertexCount() << " vertices"
            << std::endl;
  std::cout << "  Cube: " << cube->GetVertexCount() << " vertices" << std::endl;
  std::cout << "  Sphere: " << sphere->GetVertexCount() << " vertices"
            << std::endl;

  // 5. 渲染循环
  std::cout << "Starting render loop..." << std::endl;

  auto startTime = std::chrono::high_resolution_clock::now();
  int frameCount = 0;

  while (!renderer->ShouldClose()) {
    // 处理事件
    renderer->PollEvents();

    // 开始渲染
    if (renderer->BeginFrame()) {
      // 清屏
      renderer->Clear(0.2f, 0.3f, 0.3f, 1.0f);

      // 渲染几何体（暂时只是调用接口，实际渲染将在Vulkan后端实现）
      renderer->RenderMesh(*triangle);

      // 结束渲染
      renderer->EndFrame();
    }

    frameCount++;

    // 每秒输出一次FPS信息
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        currentTime - startTime);
    if (elapsed.count() >= 1) {
      std::cout << "FPS: " << frameCount << std::endl;
      frameCount = 0;
      startTime = currentTime;
    }
  }

  std::cout << "Render loop ended" << std::endl;

  // 6. 清理
  renderer->Shutdown();
  std::cout << "Demo completed successfully!" << std::endl;

  return 0;
}