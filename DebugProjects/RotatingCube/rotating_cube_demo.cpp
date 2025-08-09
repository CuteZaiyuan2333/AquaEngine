#include <AquaVisual/AquaVisualMVP.h>
#include <chrono>
#include <iostream>
#include <cmath>

using namespace AquaVisual;

class RotatingCubeDemo {
private:
    std::unique_ptr<Renderer> renderer;
    std::shared_ptr<Mesh> cubeMesh;
    Camera camera;
    
public:
    RotatingCubeDemo() = default;
    
    bool Initialize() {
        std::cout << "Initializing AquaVisual MVP..." << std::endl;
        
        // Initialize AquaVisual MVP
        if (!InitializeMVP()) {
            std::cerr << "Failed to initialize AquaVisual MVP!" << std::endl;
            return false;
        }
        std::cout << "AquaVisual MVP initialized successfully!" << std::endl;

        // Create renderer configuration
        RendererConfig config;
        config.width = 800;
        config.height = 600;
        config.title = "AquaVisual Rotating Colorful Cube Demo";
        config.enableValidation = true;
        config.enableVSync = true;

        // Create renderer
        renderer = Renderer::Create(config);
        if (!renderer) {
            std::cerr << "Failed to create renderer!" << std::endl;
            ShutdownMVP();
            return false;
        }
        std::cout << "Renderer created successfully!" << std::endl;

        // Create camera
        camera.SetPosition(Vec3(4.0f, 4.0f, 4.0f));
        camera.SetTarget(Vec3(0.0f, 0.0f, 0.0f));
        camera.SetFOV(45.0f);
        camera.SetAspectRatio(800.0f / 600.0f);
        renderer->SetCamera(camera);
        std::cout << "Camera configured!" << std::endl;

        // Create colorful cube mesh
        cubeMesh = Primitives::CreateCube(2.0f);
        if (!cubeMesh) {
            std::cerr << "Failed to create cube mesh!" << std::endl;
            renderer->Shutdown();
            ShutdownMVP();
            return false;
        }
        std::cout << "Colorful cube mesh created!" << std::endl;
        
        std::cout << "Initialization completed successfully!" << std::endl;
        return true;
    }
    
    void Update() {
        static float rotationAngle = 0.0f;
        rotationAngle += 0.02f;
        
        if (rotationAngle > 2.0f * 3.14159f) {
            rotationAngle = 0.0f;
        }
        
        float cameraRadius = 5.0f;
        float x = cameraRadius * std::cos(rotationAngle * 0.7f);
        float y = 2.0f + std::sin(rotationAngle * 0.5f);
        float z = cameraRadius * std::sin(rotationAngle * 0.7f);
        
        camera.SetPosition(Vec3(x, y, z));
        camera.SetTarget(Vec3(0.0f, 0.0f, 0.0f));
        renderer->SetCamera(camera);
    }
    
    void Render() {
        renderer->PollEvents();

        if (renderer->BeginFrame()) {
            renderer->Clear(0.1f, 0.1f, 0.2f, 1.0f);
            renderer->RenderMesh(*cubeMesh);
            renderer->EndFrame();
        }
    }
    
    void Run() {
        std::cout << "Starting rotating colorful cube demo..." << std::endl;
        std::cout << "Press ESC or close window to exit" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        int frameCount = 0;
        
        while (!renderer->ShouldClose()) {
            Update();
            Render();
            
            frameCount++;
            
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                currentTime - startTime);
            if (elapsed.count() >= 1) {
                std::cout << "FPS: " << frameCount << " | Cube rotating smoothly!" << std::endl;
                frameCount = 0;
                startTime = currentTime;
            }
        }
        
        std::cout << "Render loop ended" << std::endl;
    }

    void Cleanup() {
        if (renderer) {
            renderer->Shutdown();
        }
        ShutdownMVP();
    }
};

int main() {
    std::cout << "=== AquaVisual Rotating Cube Demo ===" << std::endl;
    std::cout << "Powered by AquaVisual Rendering Engine" << std::endl;
    std::cout << "=======================================" << std::endl;

    RotatingCubeDemo demo;

    if (!demo.Initialize()) {
        std::cerr << "Failed to initialize demo!" << std::endl;
        return -1;
    }

    try {
        demo.Run();
    }
    catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        demo.Cleanup();
        return -1;
    }

    demo.Cleanup();
    std::cout << "Demo completed successfully!" << std::endl;
    return 0;
}