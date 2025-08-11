#version 450

// 顶点属性
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// 输出到片段着色器
layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragWorldPos;

// 变换矩阵UBO
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 normalMatrix;  // 法线变换矩阵
} ubo;

void main() {
    // 计算世界空间位置
    vec4 worldPos = ubo.model * vec4(inPosition, 1.0);
    fragWorldPos = worldPos.xyz;
    
    // 变换法线到世界空间
    fragNormal = normalize(mat3(ubo.normalMatrix) * inNormal);
    
    // 传递纹理坐标
    fragTexCoord = inTexCoord;
    
    // 计算最终位置
    gl_Position = ubo.proj * ubo.view * worldPos;
}