#pragma once

#include <memory>
#include <string>
#include <vector>

namespace AquaVisual {

/**
 * @brief Texture format enumeration
 */
enum class TextureFormat {
  R8,
  RG8,
  RGB8,
  RGBA8,
  R16F,
  RG16F,
  RGB16F,
  RGBA16F,
  R32F,
  RG32F,
  RGB32F,
  RGBA32F
};

/**
 * @brief Texture filtering mode
 */
enum class TextureFilter { Nearest, Linear };

/**
 * @brief Texture wrapping mode
 */
enum class TextureWrap { Repeat, ClampToEdge, ClampToBorder };

/**
 * @brief Texture parameters
 */
struct TextureParams {
  TextureFilter minFilter = TextureFilter::Linear;
  TextureFilter magFilter = TextureFilter::Linear;
  TextureWrap wrapS = TextureWrap::Repeat;
  TextureWrap wrapT = TextureWrap::Repeat;
  bool generateMipmaps = true;
};

/**
 * @brief Texture class - MVP version
 */
class Texture {
public:
  /**
   * @brief Constructor
   * @param width Texture width
   * @param height Texture height
   * @param format Texture format
   * @param data Texture data
   * @param params Texture parameters
   */
  Texture(uint32_t width, uint32_t height, TextureFormat format,
          const void *data = nullptr, const TextureParams &params = {});

  /**
   * @brief Destructor
   */
  virtual ~Texture() = default;

  /**
   * @brief Get texture width
   * @return Texture width
   */
  uint32_t GetWidth() const { return m_width; }

  /**
   * @brief Get texture height
   * @return Texture height
   */
  uint32_t GetHeight() const { return m_height; }

  /**
   * @brief Get texture format
   * @return Texture format
   */
  TextureFormat GetFormat() const { return m_format; }

  /**
   * @brief Get texture parameters
   * @return Texture parameters
   */
  const TextureParams &GetParams() const { return m_params; }

  /**
   * @brief Create texture from file
   * @param filepath File path
   * @param params Texture parameters
   * @return Texture object
   */
  static std::unique_ptr<Texture>
  CreateFromFile(const std::string &filepath, const TextureParams &params = {});

  /**
   * @brief Create solid color texture
   * @param width Texture width
   * @param height Texture height
   * @param r Red component (0-255)
   * @param g Green component (0-255)
   * @param b Blue component (0-255)
   * @param a Alpha component (0-255)
   * @param params Texture parameters
   * @return Texture object
   */
  static std::unique_ptr<Texture> CreateSolid(uint32_t width, uint32_t height,
                                              uint8_t r, uint8_t g, uint8_t b,
                                              uint8_t a = 255,
                                              const TextureParams &params = {});

  /**
   * @brief Create checkerboard texture
   * @param width Texture width
   * @param height Texture height
   * @param tileSize Tile size
   * @param params Texture parameters
   * @return Texture object
   */
  static std::unique_ptr<Texture>
  CreateCheckerboard(uint32_t width, uint32_t height, uint32_t tileSize = 32,
                     const TextureParams &params = {});

  /**
   * @brief Get format byte size
   * @param format Texture format
   * @return Bytes per pixel
   */
  static uint32_t GetFormatSize(TextureFormat format);

protected:
  uint32_t m_width;
  uint32_t m_height;
  TextureFormat m_format;
  TextureParams m_params;
};

} // namespace AquaVisual