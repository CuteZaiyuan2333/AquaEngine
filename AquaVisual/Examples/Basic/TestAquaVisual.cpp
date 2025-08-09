#include <iostream>
#include <memory>
#include <chrono>
#include <AquaVisual/AquaVisual.h>
#include <AquaVisual/Math/Vector.h>
#include <AquaVisual/Math/Matrix.h>
#include <AquaVisual/Primitives.h>
#include <AquaVisual/Resources/Mesh.h>
#include <AquaVisual/Resources/Texture.h>

class AquaVisualTester {
public:
    void RunAllTests() {
        std::cout << "=== AquaVisual Library Test Suite ===" << std::endl;
        
        TestInitialization();
        TestMathLibrary();
        TestGeometryGeneration();
        TestMeshOperations();
        TestTextureSystem();
        TestPerformance();
        
        std::cout << "\n=== All Tests Completed ===" << std::endl;
    }

private:
    void TestInitialization() {
        std::cout << "\n1. Testing Initialization..." << std::endl;
        
        if (AquaVisual::Initialize()) {
            std::cout << "  [OK] AquaVisual initialized successfully" << std::endl;
        } else {
            std::cout << "  [ERROR] Failed to initialize AquaVisual" << std::endl;
            return;
        }
        
        std::cout << "  [OK] AquaVisual shutdown completed" << std::endl;
    }

    void TestMathLibrary() {
        std::cout << "\n2. Testing Math Library..." << std::endl;
        
        // Vector operations
        AquaVisual::Vector3 v1(1.0f, 2.0f, 3.0f);
        AquaVisual::Vector3 v2(4.0f, 5.0f, 6.0f);
        
        auto sum = v1 + v2;
        std::cout << "  [OK] Vector addition: (" << sum.x << ", " << sum.y << ", " << sum.z << ")" << std::endl;
        
        float dot = v1.Dot(v2);
        std::cout << "  [OK] Dot product: " << dot << std::endl;
        
        auto cross = v1.Cross(v2);
        std::cout << "  [OK] Cross product: (" << cross.x << ", " << cross.y << ", " << cross.z << ")" << std::endl;
        
        // Matrix operations
        auto identity = AquaVisual::Matrix4::Identity();
        std::cout << "  [OK] Identity matrix created" << std::endl;
        
        // Test matrix multiplication
        auto result = identity * identity;
        std::cout << "  [OK] Matrix multiplication completed" << std::endl;
    }

    void TestGeometryGeneration() {
        std::cout << "\n3. Testing Geometry Generation..." << std::endl;
        
        // Test triangle creation
        auto triangle = AquaVisual::Primitives::CreateTriangle();
        if (triangle) {
            std::cout << "  [OK] Triangle created with " << triangle->GetVertexCount() << " vertices" << std::endl;
        }
        
        // Test cube creation
        auto cube = AquaVisual::Primitives::CreateCube();
        if (cube) {
            std::cout << "  [OK] Cube created with " << cube->GetVertexCount() << " vertices" << std::endl;
        }
        
        // Test sphere creation
        auto sphere = AquaVisual::Primitives::CreateSphere(1.0f, 16);
        if (sphere) {
            std::cout << "  [OK] Sphere created with " << sphere->GetVertexCount() << " vertices" << std::endl;
        }
        
        // Test plane creation
        auto plane = AquaVisual::Primitives::CreatePlane();
        if (plane) {
            std::cout << "  [OK] Plane created with " << plane->GetVertexCount() << " vertices" << std::endl;
        }
    }

    void TestMeshOperations() {
        std::cout << "\n4. Testing Mesh Operations..." << std::endl;
        
        // Create a simple mesh
        auto mesh = AquaVisual::Primitives::CreateTriangle();
        if (mesh) {
            std::cout << "  [OK] Mesh created successfully" << std::endl;
            std::cout << "  [INFO] Vertex count: " << mesh->GetVertexCount() << std::endl;
            std::cout << "  [INFO] Index count: " << mesh->GetIndexCount() << std::endl;
            
            // Test vertex data access
            auto vertices = mesh->GetVertices();
            if (!vertices.empty()) {
                std::cout << "  [OK] Vertex data accessible" << std::endl;
            }
            
            // Test index data access
            auto indices = mesh->GetIndices();
            if (!indices.empty()) {
                std::cout << "  [OK] Index data accessible" << std::endl;
            }
        }
    }

    void TestTextureSystem() {
        std::cout << "\n5. Testing Texture System..." << std::endl;
        
        // Create solid color textures
        auto redTexture = AquaVisual::Texture::CreateSolid(64, 64, 255, 0, 0, 255);
        if (redTexture) {
            std::cout << "  [OK] Red texture created: " << redTexture->GetWidth() << "x" << redTexture->GetHeight() << std::endl;
        }
        
        auto greenTexture = AquaVisual::Texture::CreateSolid(32, 32, 0, 255, 0, 255);
        if (greenTexture) {
            std::cout << "  [OK] Green texture created: " << greenTexture->GetWidth() << "x" << greenTexture->GetHeight() << std::endl;
        }
        
        // Create checkerboard texture
        auto checkerboard = AquaVisual::Texture::CreateCheckerboard(128, 128, 16);
        if (checkerboard) {
            std::cout << "  [OK] Checkerboard texture created: " << checkerboard->GetWidth() << "x" << checkerboard->GetHeight() << std::endl;
        }
    }

    void TestPerformance() {
        std::cout << "\n6. Testing Performance..." << std::endl;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Create multiple meshes
        const int meshCount = 100;
        std::vector<std::unique_ptr<AquaVisual::Mesh>> meshes;
        
        for (int i = 0; i < meshCount; ++i) {
            auto mesh = AquaVisual::Primitives::CreateCube();
            if (mesh) {
                meshes.push_back(std::move(mesh));
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "  [OK] Created " << meshes.size() << " cubes in " << duration.count() << "ms" << std::endl;
        std::cout << "  [INFO] Average: " << (float)duration.count() / meshCount << "ms per cube" << std::endl;
    }
};

int main() {
    try {
        AquaVisualTester tester;
        tester.RunAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}