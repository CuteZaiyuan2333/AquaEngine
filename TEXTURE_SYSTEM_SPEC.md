# 纹理系统技术规范 🎨

## 🎯 目标

实现完整的纹理系统，让AquaEngine从彩色三角形跃升到真实的纹理渲染。

## 🏗️ 架构设计

### 核心类结构
```cpp
namespace Aqua::Renderer {

// 纹理类
class Texture {
public:
    enum class Format {
        R8G8B8A8_UNORM,
        R8G8B8_UNORM,
        R16G16B16A16_SFLOAT,
        D32_SFLOAT
    };
    
    enum class Filter {
        Nearest,
        Linear
    };
    
    enum class WrapMode {
        Repeat,
        ClampToEdge,
        ClampToBorder
    };

private:
    VkImage m_image;
    VkImageView m_imageView;
    VkDeviceMemory m_imageMemory;
    VkSampler m_sampler;
    
    uint32_t m_width, m_height;
    uint32_t m_mipLevels;
    Format m_format;

public:
    Texture();
    ~Texture();
    
    // 从文件加载
    bool LoadFromFile(const std::string& filepath);
    
    // 从内存数据创建
    bool CreateFromData(const void* data, uint32_t width, uint32_t height, Format format);
    
    // 创建空纹理
    bool Create(uint32_t width, uint32_t height, Format format, bool generateMips = true);
    
    // 绑定到描述符集
    void Bind(VkDescriptorSet descriptorSet, uint32_t binding);
    
    // 获取Vulkan对象
    VkImageView GetImageView() const { return m_imageView; }
    VkSampler GetSampler() const { return m_sampler; }
    
    // 属性访问
    uint32_t GetWidth() const { return m_width; }
    uint32_t GetHeight() const { return m_height; }
    Format GetFormat() const { return m_format; }
};

// 纹理管理器
class TextureManager {
private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    std::shared_ptr<VulkanContext> m_vulkanContext;

public:
    TextureManager(std::shared_ptr<VulkanContext> context);
    ~TextureManager();
    
    // 加载纹理（带缓存）
    std::shared_ptr<Texture> LoadTexture(const std::string& filepath);
    
    // 创建纹理
    std::shared_ptr<Texture> CreateTexture(const std::string& name, uint32_t width, uint32_t height, Texture::Format format);
    
    // 获取默认纹理
    std::shared_ptr<Texture> GetWhiteTexture();
    std::shared_ptr<Texture> GetBlackTexture();
    std::shared_ptr<Texture> GetNormalTexture();
    
    // 清理未使用的纹理
    void CleanupUnused();
};

} // namespace Aqua::Renderer
```

## 📁 文件结构

```
Core/Renderer/
├── Texture.h              # 纹理类声明
├── Texture.cpp            # 纹理类实现
├── TextureManager.h       # 纹理管理器声明
├── TextureManager.cpp     # 纹理管理器实现
└── TextureUtils.h         # 纹理工具函数
```

## 🔧 实现细节

### 1. 图像加载 (STB_image集成)

```cpp
// TextureUtils.h
namespace Aqua::Renderer::TextureUtils {
    struct ImageData {
        unsigned char* pixels;
        int width, height, channels;
        
        ~ImageData() {
            if (pixels) {
                stbi_image_free(pixels);
            }
        }
    };
    
    std::unique_ptr<ImageData> LoadImageFromFile(const std::string& filepath);
    Texture::Format GetFormatFromChannels(int channels);
}

// Texture.cpp
bool Texture::LoadFromFile(const std::string& filepath) {
    auto imageData = TextureUtils::LoadImageFromFile(filepath);
    if (!imageData || !imageData->pixels) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        return false;
    }
    
    m_width = imageData->width;
    m_height = imageData->height;
    m_format = TextureUtils::GetFormatFromChannels(imageData->channels);
    
    return CreateFromData(imageData->pixels, m_width, m_height, m_format);
}
```

### 2. Vulkan纹理创建

```cpp
bool Texture::CreateFromData(const void* data, uint32_t width, uint32_t height, Format format) {
    m_width = width;
    m_height = height;
    m_format = format;
    m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
    
    VkDeviceSize imageSize = width * height * GetBytesPerPixel(format);
    
    // 创建暂存缓冲区
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer, stagingBufferMemory);
    
    // 复制数据到暂存缓冲区
    void* mappedData;
    vkMapMemory(m_device, stagingBufferMemory, 0, imageSize, 0, &mappedData);
    memcpy(mappedData, data, static_cast<size_t>(imageSize));
    vkUnmapMemory(m_device, stagingBufferMemory);
    
    // 创建图像
    CreateImage(width, height, m_mipLevels, VK_SAMPLE_COUNT_1_BIT, 
               GetVulkanFormat(format), VK_IMAGE_TILING_OPTIMAL,
               VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_image, m_imageMemory);
    
    // 转换图像布局并复制数据
    TransitionImageLayout(m_image, GetVulkanFormat(format), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mipLevels);
    CopyBufferToImage(stagingBuffer, m_image, width, height);
    
    // 生成Mipmap
    GenerateMipmaps(m_image, GetVulkanFormat(format), width, height, m_mipLevels);
    
    // 清理暂存缓冲区
    vkDestroyBuffer(m_device, stagingBuffer, nullptr);
    vkFreeMemory(m_device, stagingBufferMemory, nullptr);
    
    // 创建图像视图
    CreateImageView();
    
    // 创建采样器
    CreateSampler();
    
    return true;
}
```

### 3. Mipmap生成

```cpp
void Texture::GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
    // 检查格式是否支持线性过滤
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_physicalDevice, imageFormat, &formatProperties);
    
    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        throw std::runtime_error("Texture image format does not support linear blitting!");
    }
    
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands();
    
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;
    
    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;
    
    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                           0, nullptr, 0, nullptr, 1, &barrier);
        
        VkImageBlit blit{};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;
        
        vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                      image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
        
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                           0, nullptr, 0, nullptr, 1, &barrier);
        
        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }
    
    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                       0, nullptr, 0, nullptr, 1, &barrier);
    
    EndSingleTimeCommands(commandBuffer);
}
```

## 🎨 着色器更新

### 顶点着色器 (textured.vert)
```glsl
#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;  // 新增纹理坐标

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;  // 输出纹理坐标

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}
```

### 片段着色器 (textured.frag)
```glsl
#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;  // 纹理采样器

void main() {
    vec4 texColor = texture(texSampler, fragTexCoord);
    outColor = vec4(fragColor * texColor.rgb, texColor.a);
}
```

## 📦 依赖集成

### CMakeLists.txt 更新
```cmake
# 添加STB库
find_path(STB_INCLUDE_DIRS "stb_image.h")
if(NOT STB_INCLUDE_DIRS)
    # 如果系统没有安装，使用vcpkg或下载
    include(FetchContent)
    FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(stb)
    set(STB_INCLUDE_DIRS ${stb_SOURCE_DIR})
endif()

target_include_directories(AquaCore PRIVATE ${STB_INCLUDE_DIRS})
```

## 🧪 测试用例

### 纹理立方体示例
```cpp
// 在HelloTriangle中添加纹理测试
class TexturedCubeApp : public Aqua::Platform::Application {
private:
    std::unique_ptr<Aqua::Renderer::Renderer> m_renderer;
    std::shared_ptr<Aqua::Renderer::TextureManager> m_textureManager;
    std::shared_ptr<Aqua::Renderer::Texture> m_testTexture;

public:
    bool Initialize() override {
        if (!Application::Initialize()) return false;
        
        m_renderer = std::make_unique<Aqua::Renderer::Renderer>();
        if (!m_renderer->Initialize(GetVulkanContext())) return false;
        
        m_textureManager = std::make_shared<Aqua::Renderer::TextureManager>(GetVulkanContext());
        
        // 加载测试纹理
        m_testTexture = m_textureManager->LoadTexture("Assets/Textures/test.png");
        if (!m_testTexture) {
            std::cerr << "Failed to load test texture" << std::endl;
            return false;
        }
        
        return true;
    }
    
    void Render() override {
        if (m_renderer) {
            if (m_renderer->BeginFrame()) {
                m_renderer->RenderTexturedCube(m_testTexture);
                m_renderer->EndFrame();
            }
        }
    }
};
```

## 📊 性能考虑

### 内存管理
- 纹理数据使用设备本地内存
- 实现纹理流式加载
- 支持纹理压缩格式（DXT/BC）

### 渲染优化
- 纹理图集支持
- 纹理数组批处理
- 异步纹理加载

## 🎯 验收标准

### 功能要求
- ✅ 支持PNG、JPG、TGA格式
- ✅ 自动Mipmap生成
- ✅ 多种过滤模式
- ✅ 纹理缓存管理
- ✅ 默认纹理支持

### 性能要求
- ✅ 1024x1024纹理加载 < 100ms
- ✅ 内存使用优化
- ✅ 无内存泄漏

### 质量要求
- ✅ 正确的纹理坐标映射
- ✅ 清晰的Mipmap过渡
- ✅ 稳定的渲染效果

## 🚀 实施计划

### 第1周：基础框架
- 创建Texture和TextureManager类
- 集成STB_image库
- 实现基础图像加载

### 第2周：Vulkan集成
- 实现Vulkan纹理创建
- 添加图像视图和采样器
- 完成内存管理

### 第3周：渲染集成
- 更新着色器支持纹理
- 修改渲染器支持纹理绑定
- 实现纹理立方体渲染

### 第4周：优化和测试
- 添加Mipmap生成
- 实现纹理管理器
- 性能测试和优化

让我们开始创造第一个纹理渲染效果吧！🎨✨