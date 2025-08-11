#include "AquaVisual/Resources/Texture.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

// Include STB image library
#include "../../../ThirdParty/STB/stb_image.h"

namespace AquaVisual {

Texture::Texture(uint32_t width, uint32_t height, TextureFormat format,
                 const void *data, const TextureParams &params)
    : m_width(width), m_height(height), m_format(format), m_params(params) {}

std::unique_ptr<Texture> Texture::CreateFromFile(const std::string &filepath,
                                                 const TextureParams &params) {
  std::cout << "Attempting to load texture from: " << filepath << std::endl;
  
  // Use STB image to load the texture
  int width, height, channels;
  unsigned char* imageData = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
  
  if (!imageData) {
    // If loading fails, create a placeholder texture
    std::cout << "ERROR: Failed to load texture from: " << filepath << std::endl;
    std::cout << "STB Error: " << stbi_failure_reason() << std::endl;
    
    // Create a simple placeholder pattern (checkerboard)
    std::vector<uint8_t> data(256 * 256 * 4);
    for (int y = 0; y < 256; ++y) {
      for (int x = 0; x < 256; ++x) {
        int index = (y * 256 + x) * 4;
        bool checker = ((x / 32) + (y / 32)) % 2;
        uint8_t color = checker ? 255 : 128;
        data[index] = color;     // R
        data[index + 1] = color; // G
        data[index + 2] = color; // B
        data[index + 3] = 255;   // A
      }
    }
    
    std::cout << "Created placeholder checkerboard texture (256x256)" << std::endl;
    return std::make_unique<Texture>(256, 256, TextureFormat::RGBA8,
                                     data.data(), params);
  }
  
  std::cout << "Successfully loaded texture: " << width << "x" << height << " with " << channels << " channels" << std::endl;
  
  // Determine texture format based on channels
  TextureFormat format;
  switch (channels) {
    case 1: format = TextureFormat::R8; break;
    case 2: format = TextureFormat::RG8; break;
    case 3: format = TextureFormat::RGB8; break;
    case 4: format = TextureFormat::RGBA8; break;
    default:
      std::cout << "ERROR: Unsupported channel count: " << channels << std::endl;
      stbi_image_free(imageData);
      return nullptr;
  }
  
  auto texture = std::make_unique<Texture>(width, height, format, imageData, params);
  
  // Free STB image data
  stbi_image_free(imageData);
  
  std::cout << "Texture created successfully" << std::endl;
  return texture;
}

std::unique_ptr<Texture> Texture::CreateSolid(uint32_t width, uint32_t height,
                                              uint8_t r, uint8_t g, uint8_t b,
                                              uint8_t a,
                                              const TextureParams &params) {
  // Create solid color texture data
  std::vector<uint8_t> data(width * height * 4);
  for (size_t i = 0; i < width * height; ++i) {
    data[i * 4 + 0] = r;
    data[i * 4 + 1] = g;
    data[i * 4 + 2] = b;
    data[i * 4 + 3] = a;
  }

  return std::make_unique<Texture>(width, height, TextureFormat::RGBA8,
                                   data.data(), params);
}

std::unique_ptr<Texture>
Texture::CreateCheckerboard(uint32_t width, uint32_t height, uint32_t tileSize,
                            const TextureParams &params) {
  // Create checkerboard pattern
  std::vector<uint8_t> data(width * height * 4);

  for (uint32_t y = 0; y < height; ++y) {
    for (uint32_t x = 0; x < width; ++x) {
      bool checker = ((x / tileSize) + (y / tileSize)) % 2 == 0;
      uint8_t color = checker ? 255 : 128;

      size_t index = (y * width + x) * 4;
      data[index + 0] = color; // R
      data[index + 1] = color; // G
      data[index + 2] = color; // B
      data[index + 3] = 255;   // A
    }
  }

  return std::make_unique<Texture>(width, height, TextureFormat::RGBA8,
                                   data.data(), params);
}

uint32_t Texture::GetFormatSize(TextureFormat format) {
  switch (format) {
  case TextureFormat::R8:
    return 1;
  case TextureFormat::RG8:
    return 2;
  case TextureFormat::RGB8:
    return 3;
  case TextureFormat::RGBA8:
    return 4;
  case TextureFormat::R16F:
    return 2;
  case TextureFormat::RG16F:
    return 4;
  case TextureFormat::RGB16F:
    return 6;
  case TextureFormat::RGBA16F:
    return 8;
  case TextureFormat::R32F:
    return 4;
  case TextureFormat::RG32F:
    return 8;
  case TextureFormat::RGB32F:
    return 12;
  case TextureFormat::RGBA32F:
    return 16;
  default:
    return 4;
  }
}

} // namespace AquaVisual