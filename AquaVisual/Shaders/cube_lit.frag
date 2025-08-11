#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;

// 光照参数
const vec3 lightPos = vec3(2.0, 2.0, 2.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const vec3 viewPos = vec3(0.0, 0.0, 0.0);

// 材质参数
const float ambientStrength = 0.1;
const float specularStrength = 0.5;
const int shininess = 32;

void main() {
    // 环境光
    vec3 ambient = ambientStrength * lightColor;
    
    // 漫反射光
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragWorldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // 镜面反射光
    vec3 viewDir = normalize(viewPos - fragWorldPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;
    
    // 计算最终颜色
    vec3 result = (ambient + diffuse + specular) * fragColor;
    
    // 添加一些深度雾效果
    float distance = length(fragWorldPos - viewPos);
    float fogFactor = exp(-distance * 0.1);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    vec3 fogColor = vec3(0.1, 0.1, 0.2);
    result = mix(fogColor, result, fogFactor);
    
    outColor = vec4(result, 1.0);
}