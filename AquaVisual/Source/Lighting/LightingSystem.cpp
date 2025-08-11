#include <AquaVisual/Lighting/LightingSystem.h>
#include <cstring>
#include <iostream>

namespace AquaVisual {
namespace Lighting {

LightingSystem::LightingSystem()
    : m_device(VK_NULL_HANDLE), m_uniformBuffer(VK_NULL_HANDLE),
      m_uniformBufferMemory(VK_NULL_HANDLE),
      m_descriptorSetLayout(VK_NULL_HANDLE), m_descriptorPool(VK_NULL_HANDLE),
      m_descriptorSet(VK_NULL_HANDLE), m_needsUpdate(true) {

  // 初始化光照数据
  memset(&m_lightingData, 0, sizeof(m_lightingData));

  // 设置默认环境光
  m_lightingData.ambientColor = Vector3(0.1f, 0.1f, 0.1f);
  m_lightingData.ambientIntensity = 1.0f;

  // 设置默认视角位置
  m_lightingData.viewPosition = Vector3(0.0f, 0.0f, 0.0f);

  std::cout << "LightingSystem constructor called" << std::endl;
}

LightingSystem::~LightingSystem() {
  Cleanup();
  std::cout << "LightingSystem destructor called" << std::endl;
}

bool LightingSystem::Initialize(VkDevice device,
                                VkPhysicalDevice physicalDevice) {
  m_device = device;
  m_physicalDevice = physicalDevice;

  // 创建Uniform Buffer
  if (!CreateUniformBuffer(physicalDevice)) {
    std::cerr << "Failed to create uniform buffer" << std::endl;
    return false;
  }

  // 创建描述符集布局
  if (!CreateDescriptorSetLayout()) {
    std::cerr << "Failed to create descriptor set layout" << std::endl;
    return false;
  }

  // 创建描述符池
  if (!CreateDescriptorPool()) {
    std::cerr << "Failed to create descriptor pool" << std::endl;
    return false;
  }

  // 创建描述符集
  if (!CreateDescriptorSet()) {
    std::cerr << "Failed to create descriptor set" << std::endl;
    return false;
  }

  // 初始更新UBO
  UpdateUBO();

  std::cout << "LightingSystem initialized successfully" << std::endl;
  return true;
}

void LightingSystem::Cleanup() {
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
  }

  std::cout << "LightingSystem cleanup completed" << std::endl;
}

void LightingSystem::SetDirectionalLight(const DirectionalLight &light) {
  m_lightingData.directionalLight = light;
  m_needsUpdate = true;
  std::cout << "DirectionalLight set: direction(" << light.direction.x << ", "
            << light.direction.y << ", " << light.direction.z << ")"
            << std::endl;
}

uint32_t LightingSystem::AddPointLight(const PointLight &light) {
  if (m_lightingData.numPointLights >= MAX_POINT_LIGHTS) {
    std::cerr << "Cannot add more point lights. Maximum reached: "
              << MAX_POINT_LIGHTS << std::endl;
    return UINT32_MAX;
  }

  uint32_t index = m_lightingData.numPointLights;
  m_lightingData.pointLights[index] = light;
  m_lightingData.numPointLights++;
  m_needsUpdate = true;

  std::cout << "PointLight added at index " << index << ": position("
            << light.position.x << ", " << light.position.y << ", "
            << light.position.z << ")" << std::endl;
  return index;
}

uint32_t LightingSystem::AddSpotLight(const SpotLight &light) {
  if (m_lightingData.numSpotLights >= MAX_SPOT_LIGHTS) {
    std::cerr << "Cannot add more spot lights. Maximum reached: "
              << MAX_SPOT_LIGHTS << std::endl;
    return UINT32_MAX;
  }

  uint32_t index = m_lightingData.numSpotLights;
  m_lightingData.spotLights[index] = light;
  m_lightingData.numSpotLights++;
  m_needsUpdate = true;

  std::cout << "SpotLight added at index " << index << ": position("
            << light.position.x << ", " << light.position.y << ", "
            << light.position.z << ")" << std::endl;
  return index;
}

void LightingSystem::UpdatePointLight(uint32_t index, const PointLight &light) {
  if (index >= m_lightingData.numPointLights) {
    std::cerr << "Invalid point light index: " << index << std::endl;
    return;
  }

  m_lightingData.pointLights[index] = light;
  m_needsUpdate = true;
  std::cout << "PointLight " << index << " updated" << std::endl;
}

void LightingSystem::UpdateSpotLight(uint32_t index, const SpotLight &light) {
  if (index >= m_lightingData.numSpotLights) {
    std::cerr << "Invalid spot light index: " << index << std::endl;
    return;
  }

  m_lightingData.spotLights[index] = light;
  m_needsUpdate = true;
  std::cout << "SpotLight " << index << " updated" << std::endl;
}

void LightingSystem::RemovePointLight(uint32_t index) {
  if (index >= m_lightingData.numPointLights) {
    std::cerr << "Invalid point light index: " << index << std::endl;
    return;
  }

  // 将后面的光源向前移动
  for (uint32_t i = index; i < m_lightingData.numPointLights - 1; ++i) {
    m_lightingData.pointLights[i] = m_lightingData.pointLights[i + 1];
  }

  m_lightingData.numPointLights--;
  m_needsUpdate = true;
  std::cout << "PointLight " << index << " removed" << std::endl;
}

void LightingSystem::RemoveSpotLight(uint32_t index) {
  if (index >= m_lightingData.numSpotLights) {
    std::cerr << "Invalid spot light index: " << index << std::endl;
    return;
  }

  // 将后面的光源向前移动
  for (uint32_t i = index; i < m_lightingData.numSpotLights - 1; ++i) {
    m_lightingData.spotLights[i] = m_lightingData.spotLights[i + 1];
  }

  m_lightingData.numSpotLights--;
  m_needsUpdate = true;
  std::cout << "SpotLight " << index << " removed" << std::endl;
}

void LightingSystem::ClearAllLights() {
  m_lightingData.numPointLights = 0;
  m_lightingData.numSpotLights = 0;
  m_needsUpdate = true;
  std::cout << "All lights cleared" << std::endl;
}

void LightingSystem::SetAmbientLight(const Vector3 &color, float intensity) {
  m_lightingData.ambientColor = color;
  m_lightingData.ambientIntensity = intensity;
  m_needsUpdate = true;
}

void LightingSystem::SetViewPosition(const Vector3 &position) {
  m_lightingData.viewPosition = position;
  m_needsUpdate = true;
}

void LightingSystem::UpdateUBO() {
  if (!m_needsUpdate || m_uniformBuffer == VK_NULL_HANDLE) {
    return;
  }

  void *data;
  VkResult result = vkMapMemory(m_device, m_uniformBufferMemory, 0,
                                sizeof(LightingUBO), 0, &data);
  if (result != VK_SUCCESS) {
    std::cerr << "Failed to map uniform buffer memory" << std::endl;
    return;
  }

  memcpy(data, &m_lightingData, sizeof(LightingUBO));
  vkUnmapMemory(m_device, m_uniformBufferMemory);

  m_needsUpdate = false;
}

void LightingSystem::BindDescriptorSet(VkCommandBuffer commandBuffer,
                                       VkPipelineLayout pipelineLayout,
                                       uint32_t setIndex) {
  if (m_descriptorSet != VK_NULL_HANDLE) {
    // 如果需要更新，先更新UBO
    if (m_needsUpdate) {
      UpdateUBO();
    }

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout, setIndex, 1, &m_descriptorSet, 0,
                            nullptr);
  }
}

bool LightingSystem::CreateUniformBuffer(VkPhysicalDevice physicalDevice) {
  VkDeviceSize bufferSize = sizeof(LightingUBO);

  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = bufferSize;
  bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkResult result =
      vkCreateBuffer(m_device, &bufferInfo, nullptr, &m_uniformBuffer);
  if (result != VK_SUCCESS) {
    std::cerr << "Failed to create uniform buffer" << std::endl;
    return false;
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(m_device, m_uniformBuffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = FindMemoryType(
      memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  result =
      vkAllocateMemory(m_device, &allocInfo, nullptr, &m_uniformBufferMemory);
  if (result != VK_SUCCESS) {
    std::cerr << "Failed to allocate uniform buffer memory" << std::endl;
    return false;
  }

  result =
      vkBindBufferMemory(m_device, m_uniformBuffer, m_uniformBufferMemory, 0);
  if (result != VK_SUCCESS) {
    std::cerr << "Failed to bind uniform buffer memory" << std::endl;
    return false;
  }

  return true;
}

bool LightingSystem::CreateDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  uboLayoutBinding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings = &uboLayoutBinding;

  VkResult result = vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr,
                                                &m_descriptorSetLayout);
  if (result != VK_SUCCESS) {
    std::cerr << "Failed to create descriptor set layout" << std::endl;
    return false;
  }

  return true;
}

bool LightingSystem::CreateDescriptorPool() {
  VkDescriptorPoolSize poolSize{};
  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = 1;

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes = &poolSize;
  poolInfo.maxSets = 1;

  VkResult result =
      vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool);
  if (result != VK_SUCCESS) {
    std::cerr << "Failed to create descriptor pool" << std::endl;
    return false;
  }

  return true;
}

bool LightingSystem::CreateDescriptorSet() {
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = m_descriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &m_descriptorSetLayout;

  VkResult result =
      vkAllocateDescriptorSets(m_device, &allocInfo, &m_descriptorSet);
  if (result != VK_SUCCESS) {
    std::cerr << "Failed to allocate descriptor set" << std::endl;
    return false;
  }

  // 立即更新描述符集
  UpdateDescriptorSet();

  return true;
}

void LightingSystem::UpdateDescriptorSet() {
  if (m_uniformBuffer == VK_NULL_HANDLE || m_descriptorSet == VK_NULL_HANDLE) {
    return;
  }

  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = m_uniformBuffer;
  bufferInfo.offset = 0;
  bufferInfo.range = sizeof(LightingUBO);

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

uint32_t LightingSystem::FindMemoryType(uint32_t typeFilter,
                                        VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }

  std::cerr << "Failed to find suitable memory type" << std::endl;
  return 0;
}

void LightingSystem::PrintLightingInfo() const {
  std::cout << "=== Lighting System Info ===" << std::endl;
  std::cout << "Ambient Light: (" << m_lightingData.ambientColor.x << ", "
            << m_lightingData.ambientColor.y << ", "
            << m_lightingData.ambientColor.z
            << ") intensity: " << m_lightingData.ambientIntensity << std::endl;

  std::cout << "View Position: (" << m_lightingData.viewPosition.x << ", "
            << m_lightingData.viewPosition.y << ", "
            << m_lightingData.viewPosition.z << ")" << std::endl;

  std::cout << "Directional Light: direction("
            << m_lightingData.directionalLight.direction.x << ", "
            << m_lightingData.directionalLight.direction.y << ", "
            << m_lightingData.directionalLight.direction.z << ") color("
            << m_lightingData.directionalLight.color.x << ", "
            << m_lightingData.directionalLight.color.y << ", "
            << m_lightingData.directionalLight.color.z
            << ") intensity: " << m_lightingData.directionalLight.intensity
            << std::endl;

  std::cout << "Point Lights: " << m_lightingData.numPointLights << "/"
            << MAX_POINT_LIGHTS << std::endl;
  for (int i = 0; i < m_lightingData.numPointLights; ++i) {
    const auto &light = m_lightingData.pointLights[i];
    std::cout << "  [" << i << "] pos(" << light.position.x << ", "
              << light.position.y << ", " << light.position.z << ") color("
              << light.color.x << ", " << light.color.y << ", " << light.color.z
              << ") intensity: " << light.intensity << std::endl;
  }

  std::cout << "Spot Lights: " << m_lightingData.numSpotLights << "/"
            << MAX_SPOT_LIGHTS << std::endl;
  for (int i = 0; i < m_lightingData.numSpotLights; ++i) {
    const auto &light = m_lightingData.spotLights[i];
    std::cout << "  [" << i << "] pos(" << light.position.x << ", "
              << light.position.y << ", " << light.position.z << ") dir("
              << light.direction.x << ", " << light.direction.y << ", "
              << light.direction.z << ") color(" << light.color.x << ", "
              << light.color.y << ", " << light.color.z
              << ") intensity: " << light.intensity
              << " cutoff: " << light.cutOff
              << " outerCutoff: " << light.outerCutOff << std::endl;
  }
  std::cout << "=========================" << std::endl;
}

const std::vector<PointLight> LightingSystem::GetPointLights() const {
  std::vector<PointLight> lights;
  for (int i = 0; i < m_lightingData.numPointLights; ++i) {
    lights.push_back(m_lightingData.pointLights[i]);
  }
  return lights;
}

const std::vector<SpotLight> LightingSystem::GetSpotLights() const {
  std::vector<SpotLight> lights;
  for (int i = 0; i < m_lightingData.numSpotLights; ++i) {
    lights.push_back(m_lightingData.spotLights[i]);
  }
  return lights;
}

void LightingSystem::UpdatePointLight(uint32_t index, const Vector3 &position,
                                      const Vector3 &color, float intensity) {
  if (index < static_cast<uint32_t>(m_lightingData.numPointLights)) {
    m_lightingData.pointLights[index].position = position;
    m_lightingData.pointLights[index].color = color;
    m_lightingData.pointLights[index].intensity = intensity;
    m_needsUpdate = true;
  }
}

void LightingSystem::UpdateSpotLight(uint32_t index, const Vector3 &position,
                                     const Vector3 &direction,
                                     const Vector3 &color, float intensity) {
  if (index < static_cast<uint32_t>(m_lightingData.numSpotLights)) {
    m_lightingData.spotLights[index].position = position;
    m_lightingData.spotLights[index].direction = direction;
    m_lightingData.spotLights[index].color = color;
    m_lightingData.spotLights[index].intensity = intensity;
    m_needsUpdate = true;
  }
}

} // namespace Lighting
} // namespace AquaVisual