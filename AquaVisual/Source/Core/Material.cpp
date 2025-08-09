#include "AquaVisual/Core/Material.h"
#include "AquaVisual/Resources/Texture.h"
#include <iostream>
#include <algorithm>

namespace AquaVisual {

Material::Material()
    : m_albedo(1.0f, 1.0f, 1.0f, 1.0f)
    , m_metallic(0.0f)
    , m_roughness(0.5f)
    , m_emission(0.0f, 0.0f, 0.0f, 0.0f)
    , m_normalScale(1.0f)
    , m_occlusionStrength(1.0f)
    , m_alphaCutoff(0.5f)
    , m_alphaMode(AlphaMode::Opaque)
    , m_doubleSided(false)
    , m_unlit(false) {
}

Material::Material(const Vector4& albedo)
    : m_albedo(albedo)
    , m_metallic(0.0f)
    , m_roughness(0.5f)
    , m_emission(0.0f, 0.0f, 0.0f, 0.0f)
    , m_normalScale(1.0f)
    , m_occlusionStrength(1.0f)
    , m_alphaCutoff(0.5f)
    , m_alphaMode(AlphaMode::Opaque)
    , m_doubleSided(false)
    , m_unlit(false) {
}

Material::~Material() {
}

void Material::SetAlbedo(const Vector4& albedo) {
    m_albedo = albedo;
}

const Vector4& Material::GetAlbedo() const {
    return m_albedo;
}

void Material::SetMetallic(float metallic) {
    m_metallic = std::max(0.0f, std::min(1.0f, metallic));
}

float Material::GetMetallic() const {
    return m_metallic;
}

void Material::SetRoughness(float roughness) {
    m_roughness = std::max(0.0f, std::min(1.0f, roughness));
}

float Material::GetRoughness() const {
    return m_roughness;
}

void Material::SetEmission(const Vector4& emission) {
    m_emission = emission;
}

const Vector4& Material::GetEmission() const {
    return m_emission;
}

void Material::SetNormalScale(float scale) {
    m_normalScale = scale;
}

float Material::GetNormalScale() const {
    return m_normalScale;
}

void Material::SetOcclusionStrength(float strength) {
    m_occlusionStrength = std::max(0.0f, std::min(1.0f, strength));
}

float Material::GetOcclusionStrength() const {
    return m_occlusionStrength;
}

void Material::SetAlphaCutoff(float cutoff) {
    m_alphaCutoff = std::max(0.0f, std::min(1.0f, cutoff));
}

float Material::GetAlphaCutoff() const {
    return m_alphaCutoff;
}

void Material::SetAlphaMode(AlphaMode mode) {
    m_alphaMode = mode;
}

AlphaMode Material::GetAlphaMode() const {
    return m_alphaMode;
}

void Material::SetDoubleSided(bool doubleSided) {
    m_doubleSided = doubleSided;
}

bool Material::IsDoubleSided() const {
    return m_doubleSided;
}

void Material::SetUnlit(bool unlit) {
    m_unlit = unlit;
}

bool Material::IsUnlit() const {
    return m_unlit;
}

void Material::SetAlbedoTexture(std::shared_ptr<Texture> texture) {
    m_albedoTexture = texture;
}

std::shared_ptr<Texture> Material::GetAlbedoTexture() const {
    return m_albedoTexture;
}

void Material::SetNormalTexture(std::shared_ptr<Texture> texture) {
    m_normalTexture = texture;
}

std::shared_ptr<Texture> Material::GetNormalTexture() const {
    return m_normalTexture;
}

void Material::SetMetallicRoughnessTexture(std::shared_ptr<Texture> texture) {
    m_metallicRoughnessTexture = texture;
}

std::shared_ptr<Texture> Material::GetMetallicRoughnessTexture() const {
    return m_metallicRoughnessTexture;
}

void Material::SetOcclusionTexture(std::shared_ptr<Texture> texture) {
    m_occlusionTexture = texture;
}

std::shared_ptr<Texture> Material::GetOcclusionTexture() const {
    return m_occlusionTexture;
}

void Material::SetEmissionTexture(std::shared_ptr<Texture> texture) {
    m_emissionTexture = texture;
}

std::shared_ptr<Texture> Material::GetEmissionTexture() const {
    return m_emissionTexture;
}

bool Material::HasTexture(TextureType type) const {
    switch (type) {
        case TextureType::Albedo:
            return m_albedoTexture != nullptr;
        case TextureType::Normal:
            return m_normalTexture != nullptr;
        case TextureType::MetallicRoughness:
            return m_metallicRoughnessTexture != nullptr;
        case TextureType::Occlusion:
            return m_occlusionTexture != nullptr;
        case TextureType::Emission:
            return m_emissionTexture != nullptr;
        default:
            return false;
    }
}

std::shared_ptr<Texture> Material::GetTexture(TextureType type) const {
    switch (type) {
        case TextureType::Albedo:
            return m_albedoTexture;
        case TextureType::Normal:
            return m_normalTexture;
        case TextureType::MetallicRoughness:
            return m_metallicRoughnessTexture;
        case TextureType::Occlusion:
            return m_occlusionTexture;
        case TextureType::Emission:
            return m_emissionTexture;
        default:
            return nullptr;
    }
}

void Material::SetTexture(TextureType type, std::shared_ptr<Texture> texture) {
    switch (type) {
        case TextureType::Albedo:
            m_albedoTexture = texture;
            break;
        case TextureType::Normal:
            m_normalTexture = texture;
            break;
        case TextureType::MetallicRoughness:
            m_metallicRoughnessTexture = texture;
            break;
        case TextureType::Occlusion:
            m_occlusionTexture = texture;
            break;
        case TextureType::Emission:
            m_emissionTexture = texture;
            break;
    }
}

void Material::RemoveTexture(TextureType type) {
    SetTexture(type, nullptr);
}

void Material::ClearAllTextures() {
    m_albedoTexture.reset();
    m_normalTexture.reset();
    m_metallicRoughnessTexture.reset();
    m_occlusionTexture.reset();
    m_emissionTexture.reset();
}

bool Material::IsTransparent() const {
    return m_alphaMode == AlphaMode::Blend || 
           (m_alphaMode == AlphaMode::Mask && m_albedo.w < 1.0f);
}

bool Material::RequiresAlphaTesting() const {
    return m_alphaMode == AlphaMode::Mask;
}

void Material::SetProperty(const std::string& name, float value) {
    m_floatProperties[name] = value;
}

void Material::SetProperty(const std::string& name, const Vector2& value) {
    m_vector2Properties[name] = value;
}

void Material::SetProperty(const std::string& name, const Vector3& value) {
    m_vector3Properties[name] = value;
}

void Material::SetProperty(const std::string& name, const Vector4& value) {
    m_vector4Properties[name] = value;
}

bool Material::GetProperty(const std::string& name, float& value) const {
    auto it = m_floatProperties.find(name);
    if (it != m_floatProperties.end()) {
        value = it->second;
        return true;
    }
    return false;
}

bool Material::GetProperty(const std::string& name, Vector2& value) const {
    auto it = m_vector2Properties.find(name);
    if (it != m_vector2Properties.end()) {
        value = it->second;
        return true;
    }
    return false;
}

bool Material::GetProperty(const std::string& name, Vector3& value) const {
    auto it = m_vector3Properties.find(name);
    if (it != m_vector3Properties.end()) {
        value = it->second;
        return true;
    }
    return false;
}

bool Material::GetProperty(const std::string& name, Vector4& value) const {
    auto it = m_vector4Properties.find(name);
    if (it != m_vector4Properties.end()) {
        value = it->second;
        return true;
    }
    return false;
}

bool Material::HasProperty(const std::string& name) const {
    return m_floatProperties.find(name) != m_floatProperties.end() ||
           m_vector2Properties.find(name) != m_vector2Properties.end() ||
           m_vector3Properties.find(name) != m_vector3Properties.end() ||
           m_vector4Properties.find(name) != m_vector4Properties.end();
}

void Material::RemoveProperty(const std::string& name) {
    m_floatProperties.erase(name);
    m_vector2Properties.erase(name);
    m_vector3Properties.erase(name);
    m_vector4Properties.erase(name);
}

void Material::ClearAllProperties() {
    m_floatProperties.clear();
    m_vector2Properties.clear();
    m_vector3Properties.clear();
    m_vector4Properties.clear();
}

std::shared_ptr<Material> Material::Clone() const {
    auto cloned = std::make_shared<Material>();
    
    // 复制基本属性
    cloned->m_albedo = m_albedo;
    cloned->m_metallic = m_metallic;
    cloned->m_roughness = m_roughness;
    cloned->m_emission = m_emission;
    cloned->m_normalScale = m_normalScale;
    cloned->m_occlusionStrength = m_occlusionStrength;
    cloned->m_alphaCutoff = m_alphaCutoff;
    cloned->m_alphaMode = m_alphaMode;
    cloned->m_doubleSided = m_doubleSided;
    cloned->m_unlit = m_unlit;
    
    // 复制纹理引用（共享纹理）
    cloned->m_albedoTexture = m_albedoTexture;
    cloned->m_normalTexture = m_normalTexture;
    cloned->m_metallicRoughnessTexture = m_metallicRoughnessTexture;
    cloned->m_occlusionTexture = m_occlusionTexture;
    cloned->m_emissionTexture = m_emissionTexture;
    
    // 复制自定义属性
    cloned->m_floatProperties = m_floatProperties;
    cloned->m_vector2Properties = m_vector2Properties;
    cloned->m_vector3Properties = m_vector3Properties;
    cloned->m_vector4Properties = m_vector4Properties;
    
    return cloned;
}

// Static factory methods
std::shared_ptr<Material> Material::CreateDefault() {
    return std::make_shared<Material>();
}

std::shared_ptr<Material> Material::CreateUnlit(const Vector3& color) {
    auto material = std::make_shared<Material>(Vector4(color.x, color.y, color.z, 1.0f));
    material->SetUnlit(true);
    return material;
}

std::shared_ptr<Material> Material::CreateMetal(const Vector3& albedo, float roughness) {
    auto material = std::make_shared<Material>(Vector4(albedo.x, albedo.y, albedo.z, 1.0f));
    material->SetMetallic(1.0f);
    material->SetRoughness(roughness);
    return material;
}

std::shared_ptr<Material> Material::CreateDielectric(const Vector3& albedo, float roughness) {
    auto material = std::make_shared<Material>(Vector4(albedo.x, albedo.y, albedo.z, 1.0f));
    material->SetMetallic(0.0f);
    material->SetRoughness(roughness);
    return material;
}

std::shared_ptr<Material> Material::CreateEmissive(const Vector3& emission) {
    auto material = std::make_shared<Material>();
    material->SetAlbedo(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    material->SetEmission(Vector4(emission.x, emission.y, emission.z, 0.0f));
    return material;
}

} // namespace AquaVisual