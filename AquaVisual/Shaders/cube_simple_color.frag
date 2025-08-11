#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;

void main() {
    // 简单的光照计算，不依赖UBO
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0)); // 固定光源方向
    vec3 normal = normalize(fragNormal);
    
    // 简单的漫反射光照
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0); // 白光
    
    // 环境光
    vec3 ambient = vec3(0.3, 0.3, 0.3);
    
    // 最终颜色
    vec3 result = (ambient + diffuse) * fragColor;
    
    outColor = vec4(result, 1.0);
}