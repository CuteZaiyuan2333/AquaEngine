#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;

// 光源结构体定义
struct DirectionalLight {
    vec3 direction;
    float intensity;
    vec3 color;
    float padding1;
};

struct PointLight {
    vec3 position;
    float intensity;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
    vec2 padding;
};

struct SpotLight {
    vec3 position;
    float intensity;
    vec3 direction;
    float cutOff;
    vec3 color;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    float padding;
};

// 光照UBO
layout(binding = 0) uniform LightingUBO {
    DirectionalLight directionalLight;
    PointLight pointLights[8];
    SpotLight spotLights[4];
    vec3 ambientColor;
    float ambientIntensity;
    vec3 viewPosition;
    float padding;
    int numPointLights;
    int numSpotLights;
    vec2 padding2;
} lighting;

// 材质参数
const float specularStrength = 0.5;
const int shininess = 32;

// 计算方向光照
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 baseColor) {
    vec3 lightDir = normalize(-light.direction);
    
    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color * light.intensity;
    
    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * light.color * light.intensity;
    
    return (diffuse + specular) * baseColor;
}

// 计算点光源
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color * light.intensity;
    
    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * light.color * light.intensity;
    
    // 衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (diffuse + specular) * baseColor;
}

// 计算聚光灯
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // 检查是否在聚光灯锥内
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color * light.intensity;
    
    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * light.color * light.intensity;
    
    // 衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return (diffuse + specular) * baseColor;
}

void main() {
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(lighting.viewPosition - fragWorldPos);
    
    // 环境光
    vec3 ambient = lighting.ambientColor * lighting.ambientIntensity * fragColor;
    
    // 方向光
    vec3 result = ambient;
    if (lighting.directionalLight.intensity > 0.0) {
        result += calculateDirectionalLight(lighting.directionalLight, norm, viewDir, fragColor);
    }
    
    // 点光源
    for (int i = 0; i < lighting.numPointLights && i < 8; ++i) {
        result += calculatePointLight(lighting.pointLights[i], norm, fragWorldPos, viewDir, fragColor);
    }
    
    // 聚光灯
    for (int i = 0; i < lighting.numSpotLights && i < 4; ++i) {
        result += calculateSpotLight(lighting.spotLights[i], norm, fragWorldPos, viewDir, fragColor);
    }
    
    // 添加一些深度雾效果
    float distance = length(fragWorldPos - lighting.viewPosition);
    float fogFactor = exp(-distance * 0.05);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    vec3 fogColor = vec3(0.1, 0.1, 0.2);
    result = mix(fogColor, result, fogFactor);
    
    outColor = vec4(result, 1.0);
}