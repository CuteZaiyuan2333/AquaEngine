#include "AquaVisual/Materials/PBRMaterial.h"
#include "AquaVisual/Math/Vector.h"
#include <iostream>
#include <cstring>
#include <array>
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace AquaVisual {
namespace Materials {

PBRMaterial::PBRMaterial() : 
    m_device(VK_NULL_HANDLE),
    m_uniformBuffer(VK_NULL_HANDLE),
    m_uniformBufferMemory(VK_NULL_HANDLE),
    m_uniformBufferMapped(nullptr),
    m_descriptorSetLayout(VK_NULL_HANDLE),
    m_descriptorPool(VK_NULL_HANDLE),
    m_descriptorSet(VK_NULL_HANDLE),
    m_needsUpdate(true) {
    // 设置默认材质参数
    m_materialData.albedo = Vector3(0.8f, 0.8f, 0.8f);
    m_materialData.metallic = 0.0f;
    m_materialData.roughness = 0.5f;
    m_materialData.ao = 1.0f;
}

PBRMaterial::~PBRMaterial() {
    Cleanup();
}

bool PBRMaterial::Initialize(VkDevice device, VkPhysicalDevice physicalDevice) {
    m_device = device;

    // 创建Uniform Buffer
    if (!CreateUniformBuffer(physicalDevice)) {
        std::cerr << "Failed to create material uniform buffer" << std::endl;
        return false;
    }

    // 创建描述符集布局
    if (!CreateDescriptorSetLayout()) {
        std::cerr << "Failed to create material descriptor set layout" << std::endl;
        return false;
    }

    // 创建描述符池
    if (!CreateDescriptorPool()) {
        std::cerr << "Failed to create material descriptor pool" << std::endl;
        return false;
    }

    // 创建描述符集
    if (!CreateDescriptorSet()) {
        std::cerr << "Failed to create material descriptor set" << std::endl;
        return false;
    }

    // 初始更新
    UpdateUBO();
    UpdateDescriptorSet();

    std::cout << "PBR Material initialized successfully" << std::endl;
    return true;
}

void PBRMaterial::Cleanup() {
    if (m_device != VK_NULL_HANDLE) {
        if (m_descriptorPool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
            m_descriptorPool = VK_NULL_HANDLE;
        }

        if (m_descriptorSetLayout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);
            m_descriptorSetLayout = VK_NULL_HANDLE;
        }

        if (m_uniformBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(m_device, m_uniformBuffer, nullptr);
            m_uniformBuffer = VK_NULL_HANDLE;
        }

        if (m_uniformBufferMemory != VK_NULL_HANDLE) {
            vkFreeMemory(m_device, m_uniformBufferMemory, nullptr);
            m_uniformBufferMemory = VK_NULL_HANDLE;
        }

        m_uniformBufferMapped = nullptr;
        m_device = VK_NULL_HANDLE;
    }
}

void PBRMaterial::SetAlbedo(const Vector3& color) {
    m_materialData.albedo = color;
    m_needsUpdate = true;
}

void PBRMaterial::SetMetallic(float metallic) {
    m_materialData.metallic = std::max(0.0f, std::min(1.0f, metallic));
    m_needsUpdate = true;
}

void PBRMaterial::SetRoughness(float roughness) {
    m_materialData.roughness = std::max(0.04f, std::min(1.0f, roughness));
    m_needsUpdate = true;
}

void PBRMaterial::SetAO(float ao) {
    m_materialData.ao = std::max(0.0f, std::min(1.0f, ao));
    m_needsUpdate = true;
}

void PBRMaterial::SetMetalPreset(const std::string& metalType) {
    if (metalType == "gold") {
        m_materialData = MaterialPresets::GetGoldMaterial();
    } else if (metalType == "silver") {
        m_materialData = MaterialPresets::GetSilverMaterial();
    } else if (metalType == "copper") {
        m_materialData = MaterialPresets::GetCopperMaterial();
    } else if (metalType == "iron") {
        m_materialData = MaterialPresets::GetIronMaterial();
    } else if (metalType == "aluminum") {
        m_materialData = MaterialPresets::GetAluminumMaterial();
    } else {
        std::cerr << "Unknown metal preset: " << metalType << std::endl;
        return;
    }
    m_needsUpdate = true;
}

void PBRMaterial::SetNonMetalPreset(const std::string& materialType) {
    if (materialType == "plastic") {
        m_materialData = MaterialPresets::GetPlasticMaterial(Vector3(0.8f, 0.8f, 0.8f));
    } else if (materialType == "rubber") {
        m_materialData = MaterialPresets::GetRubberMaterial(Vector3(0.2f, 0.2f, 0.2f));
    } else if (materialType == "wood") {
        m_materialData = MaterialPresets::GetWoodMaterial(Vector3(0.6f, 0.4f, 0.2f));
    } else if (materialType == "concrete") {
        m_materialData = MaterialPresets::GetConcreteMaterial(Vector3(0.7f, 0.7f, 0.7f));
    } else if (materialType == "fabric") {
        m_materialData = MaterialPresets::GetFabricMaterial(Vector3(0.5f, 0.5f, 0.8f));
    } else if (materialType == "glass") {
        m_materialData = MaterialPresets::GetGlassMaterial();
    } else if (materialType == "ceramic") {
        m_materialData = MaterialPresets::GetCeramicMaterial(Vector3(0.9f, 0.9f, 0.9f));
    } else if (materialType == "leather") {
        m_materialData = MaterialPresets::GetLeatherMaterial(Vector3(0.4f, 0.2f, 0.1f));
    } else {
        std::cerr << "Unknown material preset: " << materialType << std::endl;
        return;
    }
    m_needsUpdate = true;
}

void PBRMaterial::UpdateUBO() {
    if (m_needsUpdate && m_uniformBufferMapped != nullptr) {
        memcpy(m_uniformBufferMapped, &m_materialData, sizeof(PBRMaterialData));
        m_needsUpdate = false;
    }
}

void PBRMaterial::BindDescriptorSet(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, uint32_t setIndex) {
    UpdateUBO();
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, setIndex, 1, &m_descriptorSet, 0, nullptr);
}

void PBRMaterial::PrintMaterialInfo() const {
    std::cout << "=== PBR Material Info ===" << std::endl;
    std::cout << "Albedo: (" << m_materialData.albedo.x << ", " << m_materialData.albedo.y << ", " << m_materialData.albedo.z << ")" << std::endl;
    std::cout << "Metallic: " << m_materialData.metallic << std::endl;
    std::cout << "Roughness: " << m_materialData.roughness << std::endl;
    std::cout << "AO: " << m_materialData.ao << std::endl;
}

bool PBRMaterial::CreateUniformBuffer(VkPhysicalDevice physicalDevice) {
    VkDeviceSize bufferSize = sizeof(PBRMaterialData);

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &m_uniformBuffer) != VK_SUCCESS) {
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device, m_uniformBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, 
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_uniformBufferMemory) != VK_SUCCESS) {
        return false;
    }

    vkBindBufferMemory(m_device, m_uniformBuffer, m_uniformBufferMemory, 0);
    vkMapMemory(m_device, m_uniformBufferMemory, 0, bufferSize, 0, &m_uniformBufferMapped);

    return true;
}

bool PBRMaterial::CreateDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    return vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_descriptorSetLayout) == VK_SUCCESS;
}

bool PBRMaterial::CreateDescriptorPool() {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;

    return vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool) == VK_SUCCESS;
}

bool PBRMaterial::CreateDescriptorSet() {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_descriptorSetLayout;

    return vkAllocateDescriptorSets(m_device, &allocInfo, &m_descriptorSet) == VK_SUCCESS;
}

void PBRMaterial::UpdateDescriptorSet() {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = m_uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(PBRMaterialData);

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = m_descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(m_device, 1, &descriptorWrite, 0, nullptr);
}

uint32_t PBRMaterial::FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}

// MaterialPresets 实现
PBRMaterialData MaterialPresets::GetGoldMaterial() {
    PBRMaterialData material;
    material.albedo = Vector3(1.0f, 0.766f, 0.336f);
    material.metallic = 1.0f;
    material.roughness = 0.1f;
    material.ao = 1.0f;
    return material;
}

PBRMaterialData MaterialPresets::GetSilverMaterial() {
    PBRMaterialData material;
    material.albedo = Vector3(0.972f, 0.960f, 0.915f);
    material.metallic = 1.0f;
    material.roughness = 0.05f;
    material.ao = 1.0f;
    return material;
}

PBRMaterialData MaterialPresets::GetCopperMaterial() {
    PBRMaterialData material;
    material.albedo = Vector3(0.955f, 0.637f, 0.538f);
    material.metallic = 1.0f;
    material.roughness = 0.15f;
    material.ao = 1.0f;
    return material;
}

PBRMaterialData MaterialPresets::GetIronMaterial() {
    PBRMaterialData material;
    material.albedo = Vector3(0.560f, 0.570f, 0.580f);
    material.metallic = 1.0f;
    material.roughness = 0.3f;
    material.ao = 1.0f;
    return material;
}

PBRMaterialData MaterialPresets::GetAluminumMaterial() {
    PBRMaterialData material;
    material.albedo = Vector3(0.913f, 0.921f, 0.925f);
    material.metallic = 1.0f;
    material.roughness = 0.2f;
    material.ao = 1.0f;
    return material;
}

PBRMaterialData MaterialPresets::GetPlasticMaterial(const Vector3& color) {
    PBRMaterialData material;
    material.albedo = color;
    material.metallic = 0.0f;
    material.roughness = 0.6f;
    material.ao = 1.0f;
    return material;
}

PBRMaterialData MaterialPresets::GetRubberMaterial(const Vector3& color) {
    PBRMaterialData material;
    material.albedo = color;
    material.metallic = 0.0f;
    material.roughness = 0.9f;
    material.ao = 1.0f;
    return material;
}

PBRMaterialData MaterialPresets::GetWoodMaterial(const Vector3& color) {
    PBRMaterialData material;
    material.albedo = color;
    material.metallic = 0.0f;
    material.roughness = 0.8f;
    material.ao = 1.0f;
    return material;
}

PBRMaterialData MaterialPresets::GetConcreteMaterial(const Vector3& color) {
    PBRMaterialData material;
    material.albedo = color;
    material.metallic = 0.0f;
    material.roughness = 0.95f;
    material.ao = 1.0f;
    return material;
}

PBRMaterialData MaterialPresets::GetFabricMaterial(const Vector3& color) {
    PBRMaterialData material;
    material.albedo = color;
    material.metallic = 0.0f;
    material.roughness = 0.85f;
    material.ao = 1.0f;
    return material;
}

PBRMaterialData MaterialPresets::GetGlassMaterial() {
    PBRMaterialData material;
    material.albedo = Vector3(0.95f, 0.95f, 0.95f);
    material.metallic = 0.0f;
    material.roughness = 0.0f;
    material.ao = 1.0f;
    return material;
}

PBRMaterialData MaterialPresets::GetCeramicMaterial(const Vector3& color) {
    PBRMaterialData material;
    material.albedo = color;
    material.metallic = 0.0f;
    material.roughness = 0.1f;
    material.ao = 1.0f;
    return material;
}

PBRMaterialData MaterialPresets::GetLeatherMaterial(const Vector3& color) {
    PBRMaterialData material;
    material.albedo = color;
    material.metallic = 0.0f;
    material.roughness = 0.7f;
    material.ao = 1.0f;
    return material;
}

} // namespace Materials
} // namespace AquaVisual