#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;

void main() {
    // 简单的漫反射光照
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    vec3 normal = normalize(fragNormal);
    float diff = max(dot(normal, lightDir), 0.0);
    
    // 环境光
    float ambient = 0.3;
    
    // 采样纹理
    vec4 texColor = texture(texSampler, fragTexCoord);
    
    // 最终颜色
    vec3 lighting = vec3(ambient + diff);
    outColor = vec4(texColor.rgb * lighting, texColor.a);
}