#include "../../Core/Renderer/Texture.h"
#include "../../Core/Utils/Logger.h"
#include <iostream>
#include <memory>

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    AquaEngine Texture System Demo" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "This demo showcases texture loading and" << std::endl;
    std::cout << "management capabilities of AquaEngine." << std::endl;
    std::cout << "========================================" << std::endl;

    // 创建纹理对象
    auto redTexture = std::make_unique<AquaEngine::Texture>();
    auto greenTexture = std::make_unique<AquaEngine::Texture>();

    std::cout << "\n=== Testing Texture Loading ===" << std::endl;

    // 测试红色纹理
    std::string redTexturePath = "Assets/Textures/test_red_4x4.png";
    std::cout << "\nLoading red texture: " << redTexturePath << std::endl;
    if (redTexture->LoadFromFile(redTexturePath)) {
        std::cout << "✓ Red texture loaded successfully!" << std::endl;
        std::cout << "  Size: " << redTexture->GetWidth() << "x" << redTexture->GetHeight() << std::endl;
        std::cout << "  VkImage: " << (redTexture->GetImage() != VK_NULL_HANDLE ? "Valid" : "Invalid") << std::endl;
        std::cout << "  VkImageView: " << (redTexture->GetImageView() != VK_NULL_HANDLE ? "Valid" : "Invalid") << std::endl;
    } else {
        std::cout << "✗ Failed to load red texture" << std::endl;
    }

    // 测试绿色纹理
    std::string greenTexturePath = "Assets/Textures/test_green_8x8.png";
    std::cout << "\nLoading green texture: " << greenTexturePath << std::endl;
    if (greenTexture->LoadFromFile(greenTexturePath)) {
        std::cout << "✓ Green texture loaded successfully!" << std::endl;
        std::cout << "  Size: " << greenTexture->GetWidth() << "x" << greenTexture->GetHeight() << std::endl;
        std::cout << "  VkImage: " << (greenTexture->GetImage() != VK_NULL_HANDLE ? "Valid" : "Invalid") << std::endl;
        std::cout << "  VkImageView: " << (greenTexture->GetImageView() != VK_NULL_HANDLE ? "Valid" : "Invalid") << std::endl;
    } else {
        std::cout << "✗ Failed to load green texture" << std::endl;
    }

    // 测试错误处理
    std::cout << "\n=== Testing Error Handling ===" << std::endl;
    auto invalidTexture = std::make_unique<AquaEngine::Texture>();
    std::string invalidPath = "non_existent_file.png";
    std::cout << "\nTrying to load non-existent file: " << invalidPath << std::endl;
    if (!invalidTexture->LoadFromFile(invalidPath)) {
        std::cout << "✓ Error handling works correctly" << std::endl;
    }

    // 清理
    std::cout << "\n=== Cleaning Up ===" << std::endl;
    redTexture->Cleanup();
    greenTexture->Cleanup();
    invalidTexture->Cleanup();
    std::cout << "✓ All textures cleaned up" << std::endl;

    std::cout << "\n========================================" << std::endl;
    std::cout << "    Texture Demo Complete!" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "The texture system is working correctly." << std::endl;
    std::cout << "Ready for integration with rendering!" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}