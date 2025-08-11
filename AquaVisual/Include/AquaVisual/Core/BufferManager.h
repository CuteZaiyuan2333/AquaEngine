#pragma once

#include "Common.h"
#include <memory>
#include <vector>
#include <string>

#ifdef AQUA_HAS_VULKAN
#include <vulkan/vulkan.h>
#endif

namespace AquaVisual {

// Buffer type enumeration
enum class BufferType {
    Vertex,
    Index,
    Uniform,
    Storage
};

// Buffer usage flags
enum BufferUsage {
    BUFFER_USAGE_STATIC = 0x01,   // Static data, rarely updated
    BUFFER_USAGE_DYNAMIC = 0x02,  // Dynamic data, frequently updated
    BUFFER_USAGE_STREAM = 0x04    // Stream data, updated every frame
};

// Buffer base class
class AQUA_API Buffer {
public:
    Buffer() = default;
    virtual ~Buffer() = default;

    // Create buffer
    virtual bool Create(size_t size, BufferType type, uint32_t usage) = 0;
    
    // Destroy buffer
    virtual void Destroy() = 0;
    
    // Update data
    virtual bool UpdateData(const void* data, size_t size, size_t offset = 0) = 0;
    
    // Memory mapping
    virtual void* Map() = 0;
    virtual void Unmap() = 0;
    
    // Get information
    size_t GetSize() const { return m_size; }
    BufferType GetType() const { return m_type; }
    uint32_t GetUsage() const { return m_usage; }

protected:
    size_t m_size = 0;
    BufferType m_type = BufferType::Vertex;
    uint32_t m_usage = BUFFER_USAGE_STATIC;
};

// Vulkan buffer implementation
class AQUA_API VulkanBuffer : public Buffer {
public:
    VulkanBuffer();
    ~VulkanBuffer() override;

    bool Create(size_t size, BufferType type, uint32_t usage) override;
    void Destroy() override;
    bool UpdateData(const void* data, size_t size, size_t offset = 0) override;
    void* Map() override;
    void Unmap() override;
    
#ifdef AQUA_HAS_VULKAN
    VkBuffer GetVulkanBuffer() const { return m_buffer; }
    VkDeviceMemory GetVulkanMemory() const { return m_memory; }
    
    // Vulkan specific methods
    bool CreateVulkanBuffer(VkDevice device, VkPhysicalDevice physicalDevice);
    void DestroyVulkanBuffer(VkDevice device);

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    void* m_mappedData = nullptr;
    
    uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
#endif
};

// Vertex buffer
class AQUA_API VertexBuffer {
public:
    VertexBuffer() = default;
    ~VertexBuffer() = default;

    bool Create(const void* vertices, size_t size, uint32_t usage = BUFFER_USAGE_STATIC);
    void Destroy();
    bool UpdateData(const void* vertices, size_t size, size_t offset = 0);
    
    void Bind();
    void Unbind();
    
    size_t GetSize() const { return m_buffer ? m_buffer->GetSize() : 0; }
    uint32_t GetVertexCount() const { return m_vertexCount; }
    
#ifdef AQUA_HAS_VULKAN
    VkBuffer GetVulkanBuffer() const { return m_buffer ? static_cast<VulkanBuffer*>(m_buffer.get())->GetVulkanBuffer() : VK_NULL_HANDLE; }
#endif

private:
    std::shared_ptr<Buffer> m_buffer;
    uint32_t m_vertexCount = 0;
};

// Index buffer
class AQUA_API IndexBuffer {
public:
    IndexBuffer() = default;
    ~IndexBuffer() = default;

    bool Create(const uint32_t* indices, uint32_t count, uint32_t usage = BUFFER_USAGE_STATIC);
    void Destroy();
    bool UpdateData(const uint32_t* indices, uint32_t count, size_t offset = 0);
    
    void Bind();
    void Unbind();
    
    uint32_t GetIndexCount() const { return m_indexCount; }
    
#ifdef AQUA_HAS_VULKAN
    VkBuffer GetVulkanBuffer() const { return m_buffer ? static_cast<VulkanBuffer*>(m_buffer.get())->GetVulkanBuffer() : VK_NULL_HANDLE; }
#endif

private:
    std::shared_ptr<Buffer> m_buffer;
    uint32_t m_indexCount = 0;
};

// Uniform buffer
class AQUA_API UniformBuffer {
public:
    UniformBuffer() = default;
    ~UniformBuffer() = default;

    bool Create(size_t size, uint32_t usage = BUFFER_USAGE_DYNAMIC);
    void Destroy();
    bool UpdateData(const void* data, size_t size, size_t offset = 0);
    
    void Bind(uint32_t binding);
    void Unbind();
    
    size_t GetSize() const { return m_buffer ? m_buffer->GetSize() : 0; }
    
#ifdef AQUA_HAS_VULKAN
    VkBuffer GetVulkanBuffer() const { return m_buffer ? static_cast<VulkanBuffer*>(m_buffer.get())->GetVulkanBuffer() : VK_NULL_HANDLE; }
#endif

private:
    std::shared_ptr<Buffer> m_buffer;
};

// Buffer manager
class AQUA_API BufferManager {
public:
    static BufferManager& Instance();
    
    // Create buffers
    std::shared_ptr<VertexBuffer> CreateVertexBuffer(const void* vertices, size_t size, uint32_t usage = BUFFER_USAGE_STATIC);
    std::shared_ptr<IndexBuffer> CreateIndexBuffer(const uint32_t* indices, uint32_t count, uint32_t usage = BUFFER_USAGE_STATIC);
    std::shared_ptr<UniformBuffer> CreateUniformBuffer(size_t size, uint32_t usage = BUFFER_USAGE_DYNAMIC);
    
    // Create generic buffer
    std::shared_ptr<Buffer> CreateBuffer(size_t size, BufferType type, uint32_t usage = BUFFER_USAGE_STATIC);
    
    // Destroy all buffers
    void DestroyAllBuffers();
    
    // Get statistics
    size_t GetBufferCount() const { return m_buffers.size(); }
    size_t GetTotalMemoryUsage() const;

#ifdef AQUA_HAS_VULKAN
    void SetVulkanDevice(VkDevice device, VkPhysicalDevice physicalDevice);
#endif

private:
    BufferManager() = default;
    ~BufferManager() = default;
    
    std::vector<std::shared_ptr<Buffer>> m_buffers;
    
#ifdef AQUA_HAS_VULKAN
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
#endif
};

} // namespace AquaVisual