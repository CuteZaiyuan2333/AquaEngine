#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include <AquaVisual/AquaVisual.h>
#include <AquaVisual/Math/Vector.h>
#include <AquaVisual/Math/Matrix.h>
#include <AquaVisual/Primitives.h>
#include <AquaVisual/Resources/Mesh.h>
#include <AquaVisual/Resources/Texture.h>

struct SceneObject {
    std::unique_ptr<AquaVisual::Mesh> mesh;
    std::unique_ptr<AquaVisual::Texture> texture;
    AquaVisual::Vector3 position;
    AquaVisual::Vector3 rotation;
    AquaVisual::Vector3 scale;
    std::string name;
    
    SceneObject(const std::string& objName) 
        : position(0.0f, 0.0f, 0.0f)
        , rotation(0.0f, 0.0f, 0.0f)
        , scale(1.0f, 1.0f, 1.0f)
        , name(objName) {}
};

class SimpleScene {
private:
    std::vector<std::unique_ptr<SceneObject>> objects;
    
public:
    void Initialize() {
        std::cout << "=== Initializing Simple Scene ===" << std::endl;
        
        if (!AquaVisual::Initialize()) {
            throw std::runtime_error("Failed to initialize AquaVisual");
        }
        
        CreateSceneObjects();
        std::cout << "Scene initialized with " << objects.size() << " objects" << std::endl;
    }
    
    void CreateSceneObjects() {
        // Create ground plane
        auto ground = std::make_unique<SceneObject>("Ground");
        ground->mesh = AquaVisual::Primitives::CreatePlane(10.0f, 10.0f, 10, 10);
        ground->texture = AquaVisual::Texture::CreateCheckerboard(256, 256, 32);
        ground->position = AquaVisual::Vector3(0.0f, -1.0f, 0.0f);
        objects.push_back(std::move(ground));
        
        // Create some cubes
        for (int i = 0; i < 5; ++i) {
            auto cube = std::make_unique<SceneObject>("Cube_" + std::to_string(i));
            cube->mesh = AquaVisual::Primitives::CreateCube(1.0f);
            
            // Different colored textures for each cube
            uint8_t r = (i * 50) % 255;
            uint8_t g = (i * 80) % 255;
            uint8_t b = (i * 120) % 255;
            cube->texture = AquaVisual::Texture::CreateSolid(64, 64, r, g, b);
            
            cube->position = AquaVisual::Vector3(i * 2.0f - 4.0f, 0.0f, 0.0f);
            cube->scale = AquaVisual::Vector3(0.8f, 0.8f, 0.8f);
            objects.push_back(std::move(cube));
        }
        
        // Create spheres
        for (int i = 0; i < 3; ++i) {
            auto sphere = std::make_unique<SceneObject>("Sphere_" + std::to_string(i));
            sphere->mesh = AquaVisual::Primitives::CreateSphere(0.5f, 16);
            sphere->texture = AquaVisual::Texture::CreateSolid(64, 64, 255, 100 + i * 50, 100);
            sphere->position = AquaVisual::Vector3(i * 3.0f - 3.0f, 2.0f, -2.0f);
            objects.push_back(std::move(sphere));
        }
        
        // Create triangles
        for (int i = 0; i < 4; ++i) {
            auto triangle = std::make_unique<SceneObject>("Triangle_" + std::to_string(i));
            triangle->mesh = AquaVisual::Primitives::CreateTriangle(1.0f);
            triangle->texture = AquaVisual::Texture::CreateSolid(32, 32, 100, 255, 100 + i * 30);
            triangle->position = AquaVisual::Vector3(i * 1.5f - 2.25f, 1.0f, 2.0f);
            triangle->rotation = AquaVisual::Vector3(0.0f, i * 45.0f, 0.0f);
            objects.push_back(std::move(triangle));
        }
    }
    
    void PrintSceneInfo() {
        std::cout << "\n=== Scene Information ===" << std::endl;
        std::cout << "Total objects: " << objects.size() << std::endl;
        
        int totalVertices = 0;
        int totalIndices = 0;
        
        for (const auto& obj : objects) {
            std::cout << "\nObject: " << obj->name << std::endl;
            std::cout << "  Position: (" << obj->position.x << ", " << obj->position.y << ", " << obj->position.z << ")" << std::endl;
            std::cout << "  Scale: (" << obj->scale.x << ", " << obj->scale.y << ", " << obj->scale.z << ")" << std::endl;
            std::cout << "  Vertices: " << obj->mesh->GetVertexCount() << std::endl;
            std::cout << "  Indices: " << obj->mesh->GetIndexCount() << std::endl;
            
            if (obj->texture) {
                std::cout << "  Texture: " << obj->texture->GetWidth() << "x" << obj->texture->GetHeight() << std::endl;
            }
            
            totalVertices += obj->mesh->GetVertexCount();
            totalIndices += obj->mesh->GetIndexCount();
        }
        
        std::cout << "\nTotal scene statistics:" << std::endl;
        std::cout << "  Total vertices: " << totalVertices << std::endl;
        std::cout << "  Total indices: " << totalIndices << std::endl;
    }
    
    void SimulateAnimation() {
        std::cout << "\n=== Simulating Animation ===" << std::endl;
        
        // Simulate 10 frames of animation
        for (int frame = 0; frame < 10; ++frame) {
            std::cout << "Frame " << frame + 1 << ":" << std::endl;
            
            // Rotate cubes
            for (auto& obj : objects) {
                if (obj->name.find("Cube") != std::string::npos) {
                    obj->rotation.y += 5.0f; // Rotate 5 degrees per frame
                    std::cout << "  " << obj->name << " rotation: " << obj->rotation.y << " degrees" << std::endl;
                }
            }
            
            // Move spheres up and down
            for (auto& obj : objects) {
                if (obj->name.find("Sphere") != std::string::npos) {
                    float time = frame * 0.1f;
                    obj->position.y = 2.0f + sin(time) * 0.5f;
                    std::cout << "  " << obj->name << " Y position: " << obj->position.y << std::endl;
                }
            }
        }
    }
    
    void TestRayIntersection() {
        std::cout << "\n=== Testing Ray Intersection (Simulation) ===" << std::endl;
        
        // Simulate ray casting from camera to objects
        AquaVisual::Vector3 rayOrigin(0.0f, 5.0f, 5.0f);
        AquaVisual::Vector3 rayDirection(0.0f, -1.0f, -1.0f);
        rayDirection = rayDirection.Normalize();
        
        std::cout << "Ray origin: (" << rayOrigin.x << ", " << rayOrigin.y << ", " << rayOrigin.z << ")" << std::endl;
        std::cout << "Ray direction: (" << rayDirection.x << ", " << rayDirection.y << ", " << rayDirection.z << ")" << std::endl;
        
        // Simple distance-based intersection test
        for (const auto& obj : objects) {
            AquaVisual::Vector3 toObject = obj->position - rayOrigin;
            float distance = toObject.Length();
            
            if (distance < 10.0f) { // Arbitrary intersection distance
                std::cout << "  Ray intersects " << obj->name << " at distance " << distance << std::endl;
            }
        }
    }
    
    void Cleanup() {
        std::cout << "\n=== Cleaning up scene ===" << std::endl;
        objects.clear();
        AquaVisual::Shutdown();
        std::cout << "Scene cleanup completed" << std::endl;
    }
};

int main() {
    try {
        SimpleScene scene;
        
        // Initialize the scene
        scene.Initialize();
        
        // Print scene information
        scene.PrintSceneInfo();
        
        // Simulate some animation
        scene.SimulateAnimation();
        
        // Test ray intersection
        scene.TestRayIntersection();
        
        // Cleanup
        scene.Cleanup();
        
        std::cout << "\n=== Scene Demo Completed Successfully ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}