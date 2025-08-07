#include "Core/Platform/Application.h"
#include "Core/Math/Vector3.h"
#include "Core/Math/Matrix4.h"
#include <iostream>

using namespace Aqua;

class HelloTriangleApp : public Platform::Application {
public:
    HelloTriangleApp() : Platform::Application("AquaEngine - Hello Triangle", 800, 600) {}

    bool Initialize() override {
        std::cout << "初始化 Hello Triangle 应用程序..." << std::endl;
        
        // 测试数学库
        Math::Vector3 v1(1.0f, 2.0f, 3.0f);
        Math::Vector3 v2(4.0f, 5.0f, 6.0f);
        Math::Vector3 result = v1 + v2;
        
        std::cout << "数学库测试: " << v1 << " + " << v2 << " = " << result << std::endl;
        std::cout << "向量长度: " << result.Length() << std::endl;
        
        // 测试矩阵
        Math::Matrix4 identity;
        Math::Matrix4 translation = Math::Matrix4::Translation(Math::Vector3(1.0f, 2.0f, 3.0f));
        
        std::cout << "矩阵测试完成" << std::endl;
        
        return true;
    }

    void Update(float deltaTime) override {
        // 这里将来会添加游戏逻辑更新
        static float totalTime = 0.0f;
        totalTime += deltaTime;
        
        // 每秒输出一次FPS信息
        static float lastPrint = 0.0f;
        if (totalTime - lastPrint >= 1.0f) {
            std::cout << "FPS: " << (1.0f / deltaTime) << std::endl;
            lastPrint = totalTime;
        }
    }

    void Render() override {
        // 这里将来会添加渲染代码
        // 目前只是清空屏幕
    }

    void Cleanup() override {
        std::cout << "清理 Hello Triangle 应用程序..." << std::endl;
    }
};

int main() {
    std::cout << "=== AquaEngine Hello Triangle 示例 ===" << std::endl;
    std::cout << "这是AquaEngine的第一个示例程序" << std::endl;
    std::cout << "目标：创建一个简单的三角形渲染" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        HelloTriangleApp app;
        app.Run();
    }
    catch (const std::exception& e) {
        std::cerr << "应用程序异常: " << e.what() << std::endl;
        return -1;
    }

    std::cout << "应用程序正常退出" << std::endl;
    return 0;
}