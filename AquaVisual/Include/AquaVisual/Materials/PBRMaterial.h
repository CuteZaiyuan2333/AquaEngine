#pragma once

#include "AquaVisual/Math/Vector.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <string>

namespace AquaVisual {
namespace Materials {

    /**
     * @brief PBR Material Data Structure
     */
    struct PBRMaterialData {
        Vector3 albedo;     // Base color
        float metallic;     // Metallic factor (0.0 = non-metal, 1.0 = metal)
        float roughness;    // Roughness factor (0.0 = mirror, 1.0 = completely rough)
        float ao;           // Ambient occlusion
        Vector2 padding;    // GPU alignment padding
        
        PBRMaterialData() : 
            albedo(1.0f, 1.0f, 1.0f), 
            metallic(0.0f), 
            roughness(0.5f), 
            ao(1.0f), 
            padding(0.0f, 0.0f) {}
    };

    /**
     * @brief Material Presets Manager
     */
    class MaterialPresets {
    public:
        // Metal material presets
        static PBRMaterialData GetGoldMaterial();
        static PBRMaterialData GetSilverMaterial();
        static PBRMaterialData GetCopperMaterial();
        static PBRMaterialData GetIronMaterial();
        static PBRMaterialData GetAluminumMaterial();
        static PBRMaterialData GetTitaniumMaterial();
        static PBRMaterialData GetChromeMaterial();
        static PBRMaterialData GetBrassMaterial();

        // Non-metal material presets
        static PBRMaterialData GetPlasticMaterial(const Vector3& color = Vector3(0.8f, 0.8f, 0.8f));
        static PBRMaterialData GetRubberMaterial(const Vector3& color = Vector3(0.2f, 0.2f, 0.2f));
        static PBRMaterialData GetWoodMaterial(const Vector3& color = Vector3(0.6f, 0.4f, 0.2f));
        static PBRMaterialData GetConcreteMaterial(const Vector3& color = Vector3(0.7f, 0.7f, 0.7f));
        static PBRMaterialData GetFabricMaterial(const Vector3& color = Vector3(0.5f, 0.5f, 0.8f));

        // Special materials
        static PBRMaterialData GetGlassMaterial();
        static PBRMaterialData GetCeramicMaterial(const Vector3& color = Vector3(0.9f, 0.9f, 0.9f));
        static PBRMaterialData GetLeatherMaterial(const Vector3& color = Vector3(0.4f, 0.2f, 0.1f));
    };

    /**
     * @brief PBR Material Class
     */
    class PBRMaterial {
    public:
        PBRMaterial();
        ~PBRMaterial();

        // Initialization and cleanup
        bool Initialize(VkDevice device, VkPhysicalDevice physicalDevice);
        void Cleanup();

        // Basic property setters
        void SetAlbedo(const Vector3& color);
        void SetMetallic(float metallic);
        void SetRoughness(float roughness);
        void SetAO(float ao);

        // Preset materials
        void SetMetalPreset(const std::string& metalType);
        void SetNonMetalPreset(const std::string& materialType);

        // Rendering related
        void UpdateUBO();
        void BindDescriptorSet(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, uint32_t setIndex = 2);
        
        // Get descriptor set layout
        VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_descriptorSetLayout; }

        // Get material parameters
        const PBRMaterialData& GetMaterialData() const { return m_materialData; }
        
        // Debug information
        void PrintMaterialInfo() const;

    private:
        // Vulkan resources
        VkDevice m_device;
        VkBuffer m_uniformBuffer;
        VkDeviceMemory m_uniformBufferMemory;
        void* m_uniformBufferMapped;
        
        VkDescriptorSetLayout m_descriptorSetLayout;
        VkDescriptorPool m_descriptorPool;
        VkDescriptorSet m_descriptorSet;

        // Material data
        PBRMaterialData m_materialData;
        bool m_needsUpdate;

        // Internal methods
        bool CreateUniformBuffer(VkPhysicalDevice physicalDevice);
        bool CreateDescriptorSetLayout();
        bool CreateDescriptorPool();
        bool CreateDescriptorSet();
        void UpdateDescriptorSet();
        
        uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    };

} // namespace Materials
} // namespace AquaVisual