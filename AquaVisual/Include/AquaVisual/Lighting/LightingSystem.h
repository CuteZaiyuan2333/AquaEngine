#pragma once

#include <AquaVisual/Math/Vector.h>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace AquaVisual {
namespace Lighting {

/**
 * @brief 方向光结构（如太阳光）
 */
struct DirectionalLight {
        Vector3 direction = {0.0f, -1.0f, 0.0f};
        Vector3 color = {1.0f, 1.0f, 1.0f};
        float intensity = 1.0f;
        float padding = 0.0f;
        
        DirectionalLight() = default;
        DirectionalLight(const Vector3& dir, const Vector3& col, float intens)
            : direction(dir), color(col), intensity(intens) {}
    };

    struct PointLight {
        Vector3 position = {0.0f, 0.0f, 0.0f};
        float intensity = 1.0f;
        Vector3 color = {1.0f, 1.0f, 1.0f};
        float radius = 10.0f;
        
        // Attenuation parameters
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
        float padding = 0.0f;
        
        PointLight() = default;
        PointLight(const Vector3& pos, const Vector3& col, float intens, float rad = 10.0f)
            : position(pos), color(col), intensity(intens), radius(rad) {}
    };

    struct SpotLight {
        Vector3 position = {0.0f, 0.0f, 0.0f};
        float intensity = 1.0f;
        Vector3 direction = {0.0f, -1.0f, 0.0f};
        float innerCone = 30.0f;
        Vector3 color = {1.0f, 1.0f, 1.0f};
        float outerCone = 45.0f;
        
        // Attenuation parameters
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
        
        // Cone angles (cosine values)
        float cutOff = 0.9f;      // cos(25.5 degrees)
        float outerCutOff = 0.82f; // cos(35 degrees)
        float padding[3] = {0.0f, 0.0f, 0.0f};
        
        SpotLight() = default;
        SpotLight(const Vector3& pos, const Vector3& dir, const Vector3& col, 
                 float intens, float inner = 30.0f, float outer = 45.0f)
            : position(pos), direction(dir), color(col), intensity(intens),
              innerCone(inner), outerCone(outer) {}
    };

    struct LightingUBO {
        DirectionalLight directionalLight;
        PointLight pointLights[8];
        int numPointLights = 0;
        SpotLight spotLights[4];
        int numSpotLights = 0;
        Vector3 ambientColor = {0.1f, 0.1f, 0.1f};
        float ambientIntensity = 1.0f;
        Vector3 viewPosition = {0.0f, 0.0f, 0.0f};
        float padding = 0.0f;
    };

    class LightingSystem {
    public:
        static constexpr uint32_t MAX_POINT_LIGHTS = 8;
        static constexpr uint32_t MAX_SPOT_LIGHTS = 4;

        LightingSystem();
        ~LightingSystem();

        bool Initialize(VkDevice device, VkPhysicalDevice physicalDevice);
        void Cleanup();

        void SetDirectionalLight(const DirectionalLight& light);
        uint32_t AddPointLight(const PointLight& light);
        uint32_t AddSpotLight(const SpotLight& light);
        
        void UpdatePointLight(uint32_t index, const PointLight& light);
        void UpdateSpotLight(uint32_t index, const SpotLight& light);
        
        void RemovePointLight(uint32_t index);
        void RemoveSpotLight(uint32_t index);
        void ClearAllLights();

        void SetAmbientLight(const Vector3& color, float intensity = 1.0f);
        void SetViewPosition(const Vector3& position);
        void SetViewerPosition(const Vector3& position) { SetViewPosition(position); } // Alias for compatibility

        // Get light arrays
        const std::vector<PointLight> GetPointLights() const;
        const std::vector<SpotLight> GetSpotLights() const;
        
        // Overloaded update methods for convenience
        void UpdatePointLight(uint32_t index, const Vector3& position, const Vector3& color, float intensity);
        void UpdateSpotLight(uint32_t index, const Vector3& position, const Vector3& direction, const Vector3& color, float intensity);

        void UpdateUBO();
        void BindDescriptorSet(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, uint32_t setIndex = 1);
        
        VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_descriptorSetLayout; }
        
        void PrintLightingInfo() const;
        
        uint32_t GetPointLightCount() const { return m_lightingData.numPointLights; }
        uint32_t GetSpotLightCount() const { return m_lightingData.numSpotLights; }

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        VkBuffer m_uniformBuffer = VK_NULL_HANDLE;
        VkDeviceMemory m_uniformBufferMemory = VK_NULL_HANDLE;
        void* m_uniformBufferMapped = nullptr;
        
        VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
        VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
        
        LightingUBO m_lightingData;
        bool m_needsUpdate = true;
        
        bool CreateUniformBuffer(VkPhysicalDevice physicalDevice);
        bool CreateDescriptorSetLayout();
        bool CreateDescriptorPool();
        bool CreateDescriptorSet();
        void UpdateDescriptorSet();
        
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    };

} // namespace Lighting
} // namespace AquaVisual