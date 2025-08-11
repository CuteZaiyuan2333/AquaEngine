#version 450

// 顶点输入
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(push_constant) uniform PushConstants {
    float time;
    vec3 position;  // 立方体位置偏移
} pc;

layout(binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
} camera;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;



void main() {
    vec3 pos = inPosition;
    
    // 旋转矩阵 (绕Y轴旋转)
    float angle = pc.time * 0.8;
    mat3 rotY = mat3(
        cos(angle), 0.0, sin(angle),
        0.0,        1.0, 0.0,
        -sin(angle), 0.0, cos(angle)
    );
    
    // 应用旋转
    pos = rotY * pos;
    
    // 应用位置偏移
    pos += pc.position;
    
    // 使用相机的投影矩阵
    vec4 worldPos = vec4(pos, 1.0);
    vec4 viewPos = camera.view * worldPos;
    gl_Position = camera.proj * viewPos;
    
    // 设置颜色和纹理坐标
    fragColor = vec3(1.0, 1.0, 1.0); // 白色，不调制纹理
    fragTexCoord = inTexCoord;
}