#version 450

// 输入变量
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragWorldPos;

// 输出颜色
layout(location = 0) out vec4 outColor;

// 光源结构定义
struct DirectionalLight {
    vec3 direction;
    float padding1;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    float intensity;
    vec3 color;
    float radius;
};

struct SpotLight {
    vec3 position;
    float intensity;
    vec3 direction;
    float innerCone;
    vec3 color;
    float outerCone;
};

// 光照系统UBO
layout(binding = 1) uniform LightingUBO {
    DirectionalLight directionalLight;
    PointLight pointLights[8];
    int numPointLights;
    SpotLight spotLights[4];
    int numSpotLights;
    vec3 ambientColor;
    float ambientIntensity;
    vec3 viewPosition;
    float padding;
} lighting;

// 材质参数UBO
layout(binding = 2) uniform MaterialUBO {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    vec2 padding;
} material;

// 纹理采样器
layout(binding = 3) uniform sampler2D albedoTexture;
layout(binding = 4) uniform sampler2D normalTexture;
layout(binding = 5) uniform sampler2D metallicRoughnessTexture;

// 常量
const float PI = 3.14159265359;

// PBR函数
vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(normalTexture, fragTexCoord).xyz * 2.0 - 1.0;
    
    vec3 Q1 = dFdx(fragWorldPos);
    vec3 Q2 = dFdy(fragWorldPos);
    vec2 st1 = dFdx(fragTexCoord);
    vec2 st2 = dFdy(fragTexCoord);
    
    vec3 N = normalize(fragNormal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    
    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// 计算方向光贡献
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness, vec3 F0) {
    vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    // 计算辐射度
    vec3 radiance = light.color * light.intensity;
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, halfwayDir, roughness);
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    vec3 F = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    float NdotL = max(dot(normal, lightDir), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

// 计算点光源贡献
vec3 calculatePointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness, vec3 F0) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    // 计算衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    
    // 半径衰减
    float radiusAttenuation = clamp(1.0 - (distance / light.radius), 0.0, 1.0);
    attenuation *= radiusAttenuation * radiusAttenuation;
    
    vec3 radiance = light.color * light.intensity * attenuation;
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, halfwayDir, roughness);
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    vec3 F = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    float NdotL = max(dot(normal, lightDir), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

// 计算聚光灯贡献
vec3 calculateSpotLight(SpotLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness, vec3 F0) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    // 计算聚光灯锥形衰减
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = cos(radians(light.innerCone)) - cos(radians(light.outerCone));
    float intensity = clamp((theta - cos(radians(light.outerCone))) / epsilon, 0.0, 1.0);
    
    // 距离衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    
    vec3 radiance = light.color * light.intensity * attenuation * intensity;
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, halfwayDir, roughness);
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    vec3 F = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    float NdotL = max(dot(normal, lightDir), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

void main() {
    // 采样纹理
    vec3 albedo = pow(texture(albedoTexture, fragTexCoord).rgb * material.albedo, vec3(2.2));
    vec3 metallicRoughness = texture(metallicRoughnessTexture, fragTexCoord).rgb;
    float metallic = metallicRoughness.b * material.metallic;
    float roughness = metallicRoughness.g * material.roughness;
    float ao = material.ao;
    
    // 计算法线
    vec3 normal = getNormalFromMap();
    vec3 viewDir = normalize(lighting.viewPosition - fragWorldPos);
    
    // 计算基础反射率
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    // 累积光照贡献
    vec3 Lo = vec3(0.0);
    
    // 方向光
    Lo += calculateDirectionalLight(lighting.directionalLight, normal, viewDir, albedo, metallic, roughness, F0);
    
    // 点光源
    for(int i = 0; i < lighting.numPointLights && i < 8; ++i) {
        Lo += calculatePointLight(lighting.pointLights[i], fragWorldPos, normal, viewDir, albedo, metallic, roughness, F0);
    }
    
    // 聚光灯
    for(int i = 0; i < lighting.numSpotLights && i < 4; ++i) {
        Lo += calculateSpotLight(lighting.spotLights[i], fragWorldPos, normal, viewDir, albedo, metallic, roughness, F0);
    }
    
    // 环境光
    vec3 ambient = lighting.ambientColor * lighting.ambientIntensity * albedo * ao;
    vec3 color = ambient + Lo;
    
    // HDR色调映射和伽马校正
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    
    outColor = vec4(color, 1.0);
}