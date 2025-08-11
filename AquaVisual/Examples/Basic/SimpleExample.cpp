#include <iostream>
#include <memory>

// AquaVisual headers
#include "AquaVisual/AquaVisual.h"
#include "AquaVisual/Math.h"
#include "AquaVisual/Math/Vector.h"
#include "AquaVisual/Math/Matrix.h"
#include "AquaVisual/Primitives.h"
#include "AquaVisual/Resources/Mesh.h"

using namespace AquaVisual;

int main() {
    std::cout << "=== AquaVisual Simple Example ===" << std::endl;
    
    // Initialize AquaVisual
    if (!AquaVisual::Initialize()) {
        std::cerr << "Failed to initialize AquaVisual!" << std::endl;
        return -1;
    }
    
    std::cout << "AquaVisual initialized successfully!" << std::endl;
    
    // Get version information
    std::string version = AquaVisual::GetVersion();
    std::cout << "AquaVisual Version: " << version << std::endl;
    
    // Test math library
    std::cout << "\n=== Math Library Test ===" << std::endl;
    
    // Vector operations
    Vector3 vec1(1.0f, 2.0f, 3.0f);
    Vector3 vec2(4.0f, 5.0f, 6.0f);
    Vector3 result = vec1 + vec2;
    
    std::cout << "Vector addition: (" << vec1.x << ", " << vec1.y << ", " << vec1.z << ") + ("
              << vec2.x << ", " << vec2.y << ", " << vec2.z << ") = ("
              << result.x << ", " << result.y << ", " << result.z << ")" << std::endl;
    
    float dot = vec1.Dot(vec2);
    std::cout << "Dot product: " << dot << std::endl;
    
    Vector3 cross = vec1.Cross(vec2);
    std::cout << "Cross product: (" << cross.x << ", " << cross.y << ", " << cross.z << ")" << std::endl;
    
    // Matrix operations
    Matrix4 identity = Matrix4::Identity();
    Matrix4 matrix1(2.0f); // Diagonal matrix
    Matrix4 matrix2 = identity * matrix1;
    
    std::cout << "Matrix operations completed successfully!" << std::endl;
    
    // Test geometry creation
    std::cout << "\n=== Geometry Creation Test ===" << std::endl;
    
    // Create basic geometries
    auto triangle = Primitives::CreateTriangle(1.0f);
    std::cout << "Triangle created with " << triangle->GetVertexCount() << " vertices" << std::endl;
    
    auto cube = Primitives::CreateCube(2.0f);
    std::cout << "Cube created with " << cube->GetVertexCount() << " vertices" << std::endl;
    
    auto sphere = Primitives::CreateSphere(1.5f, 32);
    std::cout << "Sphere created with " << sphere->GetVertexCount() << " vertices" << std::endl;
    
    auto plane = Primitives::CreatePlane(5.0f, 5.0f, 10, 10);
    std::cout << "Plane created with " << plane->GetVertexCount() << " vertices" << std::endl;
    
    // Test mesh operations
    std::cout << "\n=== Mesh Operations Test ===" << std::endl;
    
    if (triangle) {
        std::cout << "Triangle mesh:" << std::endl;
        std::cout << "  Vertices: " << triangle->GetVertexCount() << std::endl;
        std::cout << "  Indices: " << triangle->GetIndexCount() << std::endl;
    }
    
    std::cout << "\n=== All Tests Completed Successfully! ===" << std::endl;
    
    // Cleanup
    AquaVisual::Shutdown();
    std::cout << "AquaVisual shutdown completed." << std::endl;
    
    return 0;
}