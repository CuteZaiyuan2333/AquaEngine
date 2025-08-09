#include "AquaVisual/Core/BufferManager.h"
#include <iostream>
#include <algorithm>

#ifdef AQUA_HAS_VULKAN
#include <vulkan/vulkan.h>
#endif

namespace AquaVisual {

// VulkanBuffer Implementation
VulkanBuffer::VulkanBuffer() = default;

VulkanBuffer::~VulkanBuffer() {
    Destroy();
}

bool VulkanBuffer::Create(size_t size, BufferType type, uint32_t usage) {
    m_size = size;
    m_type = type;
    m_usage = usage;
    
    std::cout << "Created VulkanBuffer: " << size << " bytes" << std::endl;
    return true;
}

void VulkanBuffer::Destroy() {
#ifdef AQUA_HAS_VULKAN
    if (m_device != VK_NULL_HANDLE) {
        DestroyVulkanBuffer(m_device);
    }
#endif
    m_size = 0;
}

bool VulkanBuffer::UpdateData(const void* data, size_t size, size_t offset) {
    if (!data) {
        std::cerr << "Error: VulkanBuffer::UpdateData - data pointer is null!" << std::endl;
        return false;
    }
    
    if (size == 0) {
        std::cerr << "Error: VulkanBuffer::UpdateData - size is zero!" << std::endl;
        return false;
    }
    
    if (m_size == 0) {
        std::cerr << "Error: VulkanBuffer::UpdateData - buffer size is zero!" << std::endl;
        return false;
    }
    
    if (offset + size > m_size) {
        std::cerr << "Error: VulkanBuffer::UpdateData - data would exceed buffer bounds!" << std::endl;
        std::cerr << "  Buffer size: " << m_size << " bytes" << std::endl;
        std::cerr << "  Offset: " << offset << " bytes" << std::endl;
        std::cerr << "  Data size: " << size << " bytes" << std::endl;
        std::cerr << "  Required size: " << (offset + size) << " bytes" << std::endl;
        return false;
    }
    
    std::cout << "Updating buffer data: " << size << " bytes at offset " << offset 
              << " (buffer size: " << m_size << " bytes)" << std::endl;
    return true;
}

void* VulkanBuffer::Map() {
#ifdef AQUA_HAS_VULKAN
    if (m_mappedData) {
        return m_mappedData;
    }
    
    if (m_device != VK_NULL_HANDLE && m_memory != VK_NULL_HANDLE) {
        VkResult result = vkMapMemory(m_device, m_memory, 0, m_size, 0, &m_mappedData);
        if (result == VK_SUCCESS) {
            return m_mappedData;
        }
    }
#endif
    
    std::cout << "Mapping buffer memory (placeholder)" << std::endl;
    return nullptr;
}

void VulkanBuffer::Unmap() {
#ifdef AQUA_HAS_VULKAN
    if (m_device != VK_NULL_HANDLE && m_memory != VK_NULL_HANDLE && m_mappedData) {
        vkUnmapMemory(m_device, m_memory);
        m_mappedData = nullptr;
    }
#endif
    
    std::cout << "Unmapping buffer memory (placeholder)" << std::endl;
}

#ifdef AQUA_HAS_VULKAN
bool VulkanBuffer::CreateVulkanBuffer(VkDevice device, VkPhysicalDevice physicalDevice) {
    m_device = device;
    
    // Placeholder for actual Vulkan buffer creation
    std::cout << "Creating actual Vulkan buffer (placeholder)" << std::endl;
    return true;
}

void VulkanBuffer::DestroyVulkanBuffer(VkDevice device) {
    if (m_buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, m_buffer, nullptr);
        m_buffer = VK_NULL_HANDLE;
    }
    
    if (m_memory != VK_NULL_HANDLE) {
        vkFreeMemory(device, m_memory, nullptr);
        m_memory = VK_NULL_HANDLE;
    }
    
    m_mappedData = nullptr;
}

uint32_t VulkanBuffer::FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    
    std::cerr << "Failed to find suitable memory type" << std::endl;
    return 0;
}
#endif

// VertexBuffer Implementation
bool VertexBuffer::Create(const void* vertices, size_t size, uint32_t usage) {
    if (!vertices) {
        std::cerr << "Error: VertexBuffer::Create - vertices pointer is null!" << std::endl;
        return false;
    }
    
    if (size == 0) {
        std::cerr << "Error: VertexBuffer::Create - size is zero!" << std::endl;
        return false;
    }
    
    // Additional safety check for reasonable size
    if (size > 100000000) { // 100MB should be more than enough for most vertex buffers
        std::cerr << "Error: VertexBuffer::Create - size seems unreasonably large: " << size << " bytes" << std::endl;
        return false;
    }
    
    std::cout << "Creating vertex buffer with " << size << " bytes" << std::endl;
    
    m_buffer = BufferManager::Instance().CreateBuffer(size, BufferType::Vertex, usage);
    if (!m_buffer) {
        std::cerr << "Error: VertexBuffer::Create - failed to create underlying buffer!" << std::endl;
        return false;
    }
    
    if (!m_buffer->UpdateData(vertices, size)) {
        std::cerr << "Error: VertexBuffer::Create - failed to update buffer data!" << std::endl;
        m_buffer.reset();
        return false;
    }
    
    m_vertexCount = static_cast<uint32_t>(size / 12); // Assume 12 bytes per vertex
    
    std::cout << "Created vertex buffer: " << m_vertexCount << " vertices" << std::endl;
    return true;
}

void VertexBuffer::Destroy() {
    m_buffer.reset();
    m_vertexCount = 0;
}

bool VertexBuffer::UpdateData(const void* vertices, size_t size, size_t offset) {
    if (!m_buffer) return false;
    return m_buffer->UpdateData(vertices, size, offset);
}

void VertexBuffer::Bind() {
    std::cout << "Binding vertex buffer" << std::endl;
}

void VertexBuffer::Unbind() {
    std::cout << "Unbinding vertex buffer" << std::endl;
}

// IndexBuffer Implementation
bool IndexBuffer::Create(const uint32_t* indices, uint32_t count, uint32_t usage) {
    if (!indices) {
        std::cerr << "Error: IndexBuffer::Create - indices pointer is null!" << std::endl;
        return false;
    }
    
    if (count == 0) {
        std::cerr << "Error: IndexBuffer::Create - index count is zero!" << std::endl;
        return false;
    }
    
    // Additional safety check for reasonable count
    if (count > 1000000) { // 1 million indices should be more than enough for most cases
        std::cerr << "Error: IndexBuffer::Create - index count seems unreasonably large: " << count << std::endl;
        return false;
    }
    
    size_t size = count * sizeof(uint32_t);
    
    std::cout << "Creating index buffer with " << count << " indices (" << size << " bytes)" << std::endl;
    
    m_buffer = BufferManager::Instance().CreateBuffer(size, BufferType::Index, usage);
    if (!m_buffer) {
        std::cerr << "Error: IndexBuffer::Create - failed to create underlying buffer!" << std::endl;
        return false;
    }
    
    if (!m_buffer->UpdateData(indices, size)) {
        std::cerr << "Error: IndexBuffer::Create - failed to update buffer data!" << std::endl;
        m_buffer.reset();
        return false;
    }
    
    m_indexCount = count;
    
    std::cout << "Created index buffer: " << m_indexCount << " indices" << std::endl;
    return true;
}

void IndexBuffer::Destroy() {
    m_buffer.reset();
    m_indexCount = 0;
}

bool IndexBuffer::UpdateData(const uint32_t* indices, uint32_t count, size_t offset) {
    if (!m_buffer) return false;
    size_t size = count * sizeof(uint32_t);
    return m_buffer->UpdateData(indices, size, offset);
}

void IndexBuffer::Bind() {
    std::cout << "Binding index buffer" << std::endl;
}

void IndexBuffer::Unbind() {
    std::cout << "Unbinding index buffer" << std::endl;
}

// UniformBuffer Implementation
bool UniformBuffer::Create(size_t size, uint32_t usage) {
    if (size == 0) return false;
    
    m_buffer = BufferManager::Instance().CreateBuffer(size, BufferType::Uniform, usage);
    if (!m_buffer) return false;
    
    std::cout << "Created uniform buffer: " << size << " bytes" << std::endl;
    return true;
}

void UniformBuffer::Destroy() {
    m_buffer.reset();
}

bool UniformBuffer::UpdateData(const void* data, size_t size, size_t offset) {
    if (!m_buffer) return false;
    return m_buffer->UpdateData(data, size, offset);
}

void UniformBuffer::Bind(uint32_t binding) {
    std::cout << "Binding uniform buffer to binding " << binding << std::endl;
}

void UniformBuffer::Unbind() {
    std::cout << "Unbinding uniform buffer" << std::endl;
}

// BufferManager Implementation
BufferManager& BufferManager::Instance() {
    static BufferManager instance;
    return instance;
}

std::shared_ptr<VertexBuffer> BufferManager::CreateVertexBuffer(const void* vertices, size_t size, uint32_t usage) {
    auto vertexBuffer = std::make_shared<VertexBuffer>();
    if (vertexBuffer->Create(vertices, size, usage)) {
        return vertexBuffer;
    }
    return nullptr;
}

std::shared_ptr<IndexBuffer> BufferManager::CreateIndexBuffer(const uint32_t* indices, uint32_t count, uint32_t usage) {
    auto indexBuffer = std::make_shared<IndexBuffer>();
    if (indexBuffer->Create(indices, count, usage)) {
        return indexBuffer;
    }
    return nullptr;
}

std::shared_ptr<UniformBuffer> BufferManager::CreateUniformBuffer(size_t size, uint32_t usage) {
    auto uniformBuffer = std::make_shared<UniformBuffer>();
    if (uniformBuffer->Create(size, usage)) {
        return uniformBuffer;
    }
    return nullptr;
}

std::shared_ptr<Buffer> BufferManager::CreateBuffer(size_t size, BufferType type, uint32_t usage) {
    auto buffer = std::make_shared<VulkanBuffer>();
    if (buffer->Create(size, type, usage)) {
#ifdef AQUA_HAS_VULKAN
        if (m_device != VK_NULL_HANDLE) {
            buffer->CreateVulkanBuffer(m_device, m_physicalDevice);
        }
#endif
        m_buffers.push_back(buffer);
        return buffer;
    }
    return nullptr;
}

void BufferManager::DestroyAllBuffers() {
    m_buffers.clear();
    std::cout << "Destroyed all buffers" << std::endl;
}

size_t BufferManager::GetTotalMemoryUsage() const {
    size_t total = 0;
    for (const auto& buffer : m_buffers) {
        total += buffer->GetSize();
    }
    return total;
}

#ifdef AQUA_HAS_VULKAN
void BufferManager::SetVulkanDevice(VkDevice device, VkPhysicalDevice physicalDevice) {
    m_device = device;
    m_physicalDevice = physicalDevice;
    std::cout << "Set Vulkan device for BufferManager" << std::endl;
}
#endif

} // namespace AquaVisual