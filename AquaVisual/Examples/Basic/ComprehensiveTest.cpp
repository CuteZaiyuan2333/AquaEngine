#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <iomanip>

// AquaVisual headers
#include "AquaVisual/AquaVisual.h"
#include "AquaVisual/Math.h"
#include "AquaVisual/Math/Vector.h"
#include "AquaVisual/Math/Matrix.h"
#include "AquaVisual/Primitives.h"
#include "AquaVisual/Resources/Mesh.h"
#include "AquaVisual/Resources/Texture.h"

using namespace AquaVisual;

class AquaVisualTester {
private:
    bool m_initialized = false;
    std::vector<std::unique_ptr<Mesh>> m_meshes;

public:
    bool Initialize() {
        std::cout << "=== AquaVisual 综合测试程序 ===" << std::endl;
        std::cout << "正在初始化 AquaVisual..." << std::endl;
        
        if (!AquaVisual::Initialize()) {
            std::cerr << "❌ AquaVisual 初始化失败!" << std::endl;
            return false;
        }
        
        m_initialized = true;
        std::cout << "✅ AquaVisual 初始化成功!" << std::endl;
        std::cout << "📦 版本: " << AquaVisual::GetVersion() << std::endl;
        return true;
    }

    void TestMathLibrary() {
        std::cout << "\n=== 数学库测试 ===" << std::endl;
        
        // Vector2 测试
        std::cout << "📐 Vector2 测试:" << std::endl;
        Vec2 v2a(3.0f, 4.0f);
        Vec2 v2b(1.0f, 2.0f);
        Vec2 v2result = v2a + v2b;
        float v2length = v2a.Length();
        
        std::cout << "  (" << v2a.x << ", " << v2a.y << ") + (" << v2b.x << ", " << v2b.y 
                  << ") = (" << v2result.x << ", " << v2result.y << ")" << std::endl;
        std::cout << "  长度: " << v2length << std::endl;
        
        // Vector3 测试
        std::cout << "📐 Vector3 测试:" << std::endl;
        Vec3 v3a(1.0f, 2.0f, 3.0f);
        Vec3 v3b(4.0f, 5.0f, 6.0f);
        Vec3 v3sum = v3a + v3b;
        Vec3 v3cross = v3a.Cross(v3b);
        float v3dot = v3a.Dot(v3b);
        
        std::cout << "  向量加法: (" << v3a.x << ", " << v3a.y << ", " << v3a.z << ") + ("
                  << v3b.x << ", " << v3b.y << ", " << v3b.z << ") = ("
                  << v3sum.x << ", " << v3sum.y << ", " << v3sum.z << ")" << std::endl;
        std::cout << "  点积: " << v3dot << std::endl;
        std::cout << "  叉积: (" << v3cross.x << ", " << v3cross.y << ", " << v3cross.z << ")" << std::endl;
        
        // Vector4 测试
        std::cout << "📐 Vector4 测试:" << std::endl;
        Vec4 v4a(1.0f, 2.0f, 3.0f, 1.0f);
        Vec4 v4b(2.0f, 3.0f, 4.0f, 1.0f);
        Vec4 v4result = v4a * 2.0f;
        
        std::cout << "  标量乘法: (" << v4a.x << ", " << v4a.y << ", " << v4a.z << ", " << v4a.w 
                  << ") * 2 = (" << v4result.x << ", " << v4result.y << ", " << v4result.z 
                  << ", " << v4result.w << ")" << std::endl;
        
        // Matrix4 测试
        std::cout << "📐 Matrix4 测试:" << std::endl;
        Matrix4 identity = Matrix4::Identity();
        Matrix4 diagonal(2.0f);
        Matrix4 result = identity * diagonal;
        
        std::cout << "  单位矩阵创建成功" << std::endl;
        std::cout << "  对角矩阵创建成功" << std::endl;
        std::cout << "  矩阵乘法计算成功" << std::endl;
    }

    void TestPrimitiveGeneration() {
        std::cout << "\n=== 几何体生成测试 ===" << std::endl;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // 创建各种几何体
        std::cout << "🔺 创建三角形..." << std::endl;
        auto triangle = Primitives::CreateTriangle(1.0f);
        if (triangle) {
            m_meshes.push_back(std::move(triangle));
            std::cout << "  ✅ 三角形: " << m_meshes.back()->GetVertexCount() 
                      << " 顶点, " << m_meshes.back()->GetIndexCount() << " 索引" << std::endl;
        }
        
        std::cout << "🔲 创建四边形..." << std::endl;
        auto quad = Primitives::CreateQuad(2.0f, 1.5f);
        if (quad) {
            m_meshes.push_back(std::move(quad));
            std::cout << "  ✅ 四边形: " << m_meshes.back()->GetVertexCount() 
                      << " 顶点, " << m_meshes.back()->GetIndexCount() << " 索引" << std::endl;
        }
        
        std::cout << "🧊 创建立方体..." << std::endl;
        auto cube = Primitives::CreateCube(2.0f);
        if (cube) {
            m_meshes.push_back(std::move(cube));
            std::cout << "  ✅ 立方体: " << m_meshes.back()->GetVertexCount() 
                      << " 顶点, " << m_meshes.back()->GetIndexCount() << " 索引" << std::endl;
        }
        
        std::cout << "🌐 创建球体..." << std::endl;
        auto sphere = Primitives::CreateSphere(1.5f, 32);
        if (sphere) {
            m_meshes.push_back(std::move(sphere));
            std::cout << "  ✅ 球体: " << m_meshes.back()->GetVertexCount() 
                      << " 顶点, " << m_meshes.back()->GetIndexCount() << " 索引" << std::endl;
        }
        
        std::cout << "📄 创建平面..." << std::endl;
        auto plane = Primitives::CreatePlane(5.0f, 5.0f, 10, 10);
        if (plane) {
            m_meshes.push_back(std::move(plane));
            std::cout << "  ✅ 平面: " << m_meshes.back()->GetVertexCount() 
                      << " 顶点, " << m_meshes.back()->GetIndexCount() << " 索引" << std::endl;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "⏱️  几何体生成耗时: " << duration.count() << " 微秒" << std::endl;
    }

    void TestMeshOperations() {
        std::cout << "\n=== 网格操作测试 ===" << std::endl;
        
        if (m_meshes.empty()) {
            std::cout << "❌ 没有可用的网格进行测试" << std::endl;
            return;
        }
        
        for (size_t i = 0; i < m_meshes.size(); ++i) {
            const auto& mesh = m_meshes[i];
            std::cout << "🔍 网格 " << (i + 1) << ":" << std::endl;
            std::cout << "  顶点数: " << mesh->GetVertexCount() << std::endl;
            std::cout << "  索引数: " << mesh->GetIndexCount() << std::endl;
            
            // 检查顶点数据
            const auto& vertices = mesh->GetVertices();
            if (!vertices.empty()) {
                const auto& firstVertex = vertices[0];
                std::cout << "  第一个顶点位置: (" 
                          << firstVertex.position.x << ", " 
                          << firstVertex.position.y << ", " 
                          << firstVertex.position.z << ")" << std::endl;
                std::cout << "  第一个顶点法线: (" 
                          << firstVertex.normal.x << ", " 
                          << firstVertex.normal.y << ", " 
                          << firstVertex.normal.z << ")" << std::endl;
                std::cout << "  第一个顶点UV: (" 
                          << firstVertex.texCoord.x << ", " 
                          << firstVertex.texCoord.y << ")" << std::endl;
            }
            
            // 检查索引数据
            const auto& indices = mesh->GetIndices();
            if (indices.size() >= 3) {
                std::cout << "  前三个索引: " << indices[0] << ", " 
                          << indices[1] << ", " << indices[2] << std::endl;
            }
        }
    }

    void TestTextureSystem() {
        std::cout << "\n=== 纹理系统测试 ===" << std::endl;
        
        // 测试纹理格式
        std::cout << "🎨 纹理格式测试:" << std::endl;
        std::cout << "  RGBA8 字节大小: " << Texture::GetFormatByteSize(TextureFormat::RGBA8) << std::endl;
        std::cout << "  RGB8 字节大小: " << Texture::GetFormatByteSize(TextureFormat::RGB8) << std::endl;
        std::cout << "  R8 字节大小: " << Texture::GetFormatByteSize(TextureFormat::R8) << std::endl;
        
        // 创建纯色纹理
        std::cout << "🎨 创建纯色纹理..." << std::endl;
        auto redTexture = Texture::CreateSolid(64, 64, 255, 0, 0, 255);
        if (redTexture) {
            std::cout << "  ✅ 红色纹理: " << redTexture->GetWidth() << "x" << redTexture->GetHeight() << std::endl;
        }
        
        auto greenTexture = Texture::CreateSolid(32, 32, 0, 255, 0, 255);
        if (greenTexture) {
            std::cout << "  ✅ 绿色纹理: " << greenTexture->GetWidth() << "x" << greenTexture->GetHeight() << std::endl;
        }
        
        // 创建棋盘格纹理
        std::cout << "🎨 创建棋盘格纹理..." << std::endl;
        auto checkerboard = Texture::CreateCheckerboard(128, 128, 8);
        if (checkerboard) {
            std::cout << "  ✅ 棋盘格纹理: " << checkerboard->GetWidth() << "x" << checkerboard->GetHeight() << std::endl;
        }
    }

    void TestPerformance() {
        std::cout << "\n=== 性能测试 ===" << std::endl;
        
        const int iterations = 1000;
        
        // 向量运算性能测试
        auto start = std::chrono::high_resolution_clock::now();
        Vec3 result(0, 0, 0);
        for (int i = 0; i < iterations; ++i) {
            Vec3 a(i * 0.1f, i * 0.2f, i * 0.3f);
            Vec3 b(i * 0.4f, i * 0.5f, i * 0.6f);
            result = result + a.Cross(b);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto vectorDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "⚡ " << iterations << " 次向量叉积运算耗时: " 
                  << vectorDuration.count() << " 微秒" << std::endl;
        
        // 矩阵运算性能测试
        start = std::chrono::high_resolution_clock::now();
        Matrix4 matResult = Matrix4::Identity();
        for (int i = 0; i < iterations; ++i) {
            Matrix4 mat(i * 0.01f);
            matResult = matResult * mat;
        }
        end = std::chrono::high_resolution_clock::now();
        auto matrixDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "⚡ " << iterations << " 次矩阵乘法运算耗时: " 
                  << matrixDuration.count() << " 微秒" << std::endl;
    }

    void PrintSummary() {
        std::cout << "\n=== 测试总结 ===" << std::endl;
        std::cout << "📊 统计信息:" << std::endl;
        std::cout << "  创建的网格数量: " << m_meshes.size() << std::endl;
        
        size_t totalVertices = 0;
        size_t totalIndices = 0;
        for (const auto& mesh : m_meshes) {
            totalVertices += mesh->GetVertexCount();
            totalIndices += mesh->GetIndexCount();
        }
        
        std::cout << "  总顶点数: " << totalVertices << std::endl;
        std::cout << "  总索引数: " << totalIndices << std::endl;
        std::cout << "  估计内存使用: " << std::fixed << std::setprecision(2) 
                  << (totalVertices * sizeof(Vertex) + totalIndices * sizeof(uint32_t)) / 1024.0f 
                  << " KB" << std::endl;
    }

    void Cleanup() {
        if (m_initialized) {
            std::cout << "\n🧹 清理资源..." << std::endl;
            m_meshes.clear();
            AquaVisual::Shutdown();
            std::cout << "✅ AquaVisual 关闭完成" << std::endl;
        }
    }

    ~AquaVisualTester() {
        Cleanup();
    }
};

int main() {
    AquaVisualTester tester;
    
    if (!tester.Initialize()) {
        return -1;
    }
    
    try {
        tester.TestMathLibrary();
        tester.TestPrimitiveGeneration();
        tester.TestMeshOperations();
        tester.TestTextureSystem();
        tester.TestPerformance();
        tester.PrintSummary();
        
        std::cout << "\n🎉 所有测试完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试过程中发生异常: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}