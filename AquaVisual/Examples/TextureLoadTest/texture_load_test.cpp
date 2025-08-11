#include "AquaVisual/Resources/Texture.h"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== AquaVisual Texture Loading Test ===" << std::endl;
    
    // Test 1: Load the cat image
    std::string imagePath = "../../Images/CuteCatImage256x256.bmp";
    std::cout << "Testing texture loading from: " << imagePath << std::endl;
    
    auto texture = AquaVisual::Texture::CreateFromFile(imagePath);
    
    if (texture) {
        std::cout << "✅ Texture loaded successfully!" << std::endl;
        std::cout << "   Width: " << texture->GetWidth() << std::endl;
        std::cout << "   Height: " << texture->GetHeight() << std::endl;
        std::cout << "   Format: " << static_cast<int>(texture->GetFormat()) << std::endl;
        
        // Check if dimensions are correct
        if (texture->GetWidth() == 256 && texture->GetHeight() == 256) {
            std::cout << "✅ Dimensions are correct (256x256)" << std::endl;
        } else {
            std::cout << "❌ Unexpected dimensions!" << std::endl;
        }
    } else {
        std::cout << "❌ Failed to load texture!" << std::endl;
    }
    
    // Test 2: Test with non-existent file (should create placeholder)
    std::cout << "\nTesting with non-existent file..." << std::endl;
    auto placeholderTexture = AquaVisual::Texture::CreateFromFile("non_existent_file.bmp");
    
    if (placeholderTexture) {
        std::cout << "✅ Placeholder texture created successfully!" << std::endl;
        std::cout << "   Width: " << placeholderTexture->GetWidth() << std::endl;
        std::cout << "   Height: " << placeholderTexture->GetHeight() << std::endl;
        std::cout << "   Format: " << static_cast<int>(placeholderTexture->GetFormat()) << std::endl;
    } else {
        std::cout << "❌ Failed to create placeholder texture!" << std::endl;
    }
    
    // Test 3: Test solid color texture
    std::cout << "\nTesting solid color texture creation..." << std::endl;
    auto solidTexture = AquaVisual::Texture::CreateSolid(128, 128, 255, 0, 0, 255);
    
    if (solidTexture) {
        std::cout << "✅ Solid color texture created successfully!" << std::endl;
        std::cout << "   Width: " << solidTexture->GetWidth() << std::endl;
        std::cout << "   Height: " << solidTexture->GetHeight() << std::endl;
        std::cout << "   Format: " << static_cast<int>(solidTexture->GetFormat()) << std::endl;
    } else {
        std::cout << "❌ Failed to create solid color texture!" << std::endl;
    }
    
    // Test 4: Test checkerboard texture
    std::cout << "\nTesting checkerboard texture creation..." << std::endl;
    auto checkerTexture = AquaVisual::Texture::CreateCheckerboard(64, 64, 8);
    
    if (checkerTexture) {
        std::cout << "✅ Checkerboard texture created successfully!" << std::endl;
        std::cout << "   Width: " << checkerTexture->GetWidth() << std::endl;
        std::cout << "   Height: " << checkerTexture->GetHeight() << std::endl;
        std::cout << "   Format: " << static_cast<int>(checkerTexture->GetFormat()) << std::endl;
    } else {
        std::cout << "❌ Failed to create checkerboard texture!" << std::endl;
    }
    
    std::cout << "\n=== Test Complete ===" << std::endl;
    return 0;
}