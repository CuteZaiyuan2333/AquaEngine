#pragma once

#include "AquaVisual/Math/Vector.h"
#include "AquaVisual/Resources/Texture.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "../Math/Vector.h"

namespace AquaVisual {

/**
 * @brief Material alpha modes
 */
enum class AlphaMode {
    Opaque,
    Mask,
    Blend
};

/**
 * @brief Texture types
 */
enum class TextureType {
    Albedo,
    Normal,
    MetallicRoughness,
    Occlusion,
    Emission
};

/**
 * @brief Material class for managing rendering properties
 */
class Material {
public:
    Material();
    Material(const Vector4& albedo);
    ~Material();

    // Basic material properties
    void SetAlbedo(const Vector4& albedo);
    const Vector4& GetAlbedo() const;

    void SetMetallic(float metallic);
    float GetMetallic() const;

    void SetRoughness(float roughness);
    float GetRoughness() const;

    void SetEmission(const Vector4& emission);
    const Vector4& GetEmission() const;

    void SetNormalScale(float scale);
    float GetNormalScale() const;

    void SetOcclusionStrength(float strength);
    float GetOcclusionStrength() const;

    void SetAlphaCutoff(float cutoff);
    float GetAlphaCutoff() const;

    void SetAlphaMode(AlphaMode mode);
    AlphaMode GetAlphaMode() const;

    void SetDoubleSided(bool doubleSided);
    bool IsDoubleSided() const;

    void SetUnlit(bool unlit);
    bool IsUnlit() const;

    // Texture management
    void SetAlbedoTexture(std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> GetAlbedoTexture() const;

    void SetNormalTexture(std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> GetNormalTexture() const;

    void SetMetallicRoughnessTexture(std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> GetMetallicRoughnessTexture() const;

    void SetOcclusionTexture(std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> GetOcclusionTexture() const;

    void SetEmissionTexture(std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> GetEmissionTexture() const;

    // Texture management by type
    bool HasTexture(TextureType type) const;
    std::shared_ptr<Texture> GetTexture(TextureType type) const;
    void SetTexture(TextureType type, std::shared_ptr<Texture> texture);
    void RemoveTexture(TextureType type);
    void ClearAllTextures();

    // Material state queries
    bool IsTransparent() const;
    bool RequiresAlphaTesting() const;

    // Custom properties
    void SetProperty(const std::string& name, float value);
    void SetProperty(const std::string& name, const Vector2& value);
    void SetProperty(const std::string& name, const Vector3& value);
    void SetProperty(const std::string& name, const Vector4& value);

    bool GetProperty(const std::string& name, float& value) const;
    bool GetProperty(const std::string& name, Vector2& value) const;
    bool GetProperty(const std::string& name, Vector3& value) const;
    bool GetProperty(const std::string& name, Vector4& value) const;

    bool HasProperty(const std::string& name) const;
    void RemoveProperty(const std::string& name);
    void ClearAllProperties();

    // Utility
    std::shared_ptr<Material> Clone() const;

    // Static factory methods
    static std::shared_ptr<Material> CreateDefault();
    static std::shared_ptr<Material> CreateUnlit(const Vector3& color = Vector3(1.0f, 1.0f, 1.0f));
    static std::shared_ptr<Material> CreateMetal(const Vector3& albedo, float roughness = 0.1f);
    static std::shared_ptr<Material> CreateDielectric(const Vector3& albedo, float roughness = 0.5f);
    static std::shared_ptr<Material> CreateEmissive(const Vector3& emission);

private:
    // Basic properties
    Vector4 m_albedo;
    float m_metallic;
    float m_roughness;
    Vector4 m_emission;
    float m_normalScale;
    float m_occlusionStrength;
    float m_alphaCutoff;
    AlphaMode m_alphaMode;
    bool m_doubleSided;
    bool m_unlit;

    // Textures
    std::shared_ptr<Texture> m_albedoTexture;
    std::shared_ptr<Texture> m_normalTexture;
    std::shared_ptr<Texture> m_metallicRoughnessTexture;
    std::shared_ptr<Texture> m_occlusionTexture;
    std::shared_ptr<Texture> m_emissionTexture;

    // Custom properties
    std::unordered_map<std::string, float> m_floatProperties;
    std::unordered_map<std::string, Vector2> m_vector2Properties;
    std::unordered_map<std::string, Vector3> m_vector3Properties;
    std::unordered_map<std::string, Vector4> m_vector4Properties;
};

} // namespace AquaVisual