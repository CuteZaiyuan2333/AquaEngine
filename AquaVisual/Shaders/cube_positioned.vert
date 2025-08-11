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

// 每个面的颜色 - 使用鲜艳的不同颜色以便观察
vec3 faceColors[6] = vec3[](
    vec3(1.0, 0.0, 0.0), // 红色
    vec3(0.0, 1.0, 0.0), // 绿色
    vec3(0.0, 0.0, 1.0), // 蓝色
    vec3(1.0, 1.0, 0.0), // 黄色
    vec3(1.0, 0.0, 1.0), // 洋红色
    vec3(0.0, 1.0, 1.0)  // 青色
);

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
    
    // 根据法线方向确定面的颜色
    vec3 normal = normalize(inNormal);
    int faceIndex = 0;
    
    // 根据法线方向确定是哪个面
    if (abs(normal.z - 1.0) < 0.1) faceIndex = 0;      // 前面 (Z+) - 红色
    else if (abs(normal.z + 1.0) < 0.1) faceIndex = 1; // 后面 (Z-) - 绿色
    else if (abs(normal.x + 1.0) < 0.1) faceIndex = 2; // 左面 (X-) - 蓝色
    else if (abs(normal.x - 1.0) < 0.1) faceIndex = 3; // 右面 (X+) - 黄色
    else if (abs(normal.y + 1.0) < 0.1) faceIndex = 4; // 底面 (Y-) - 洋红色
    else if (abs(normal.y - 1.0) < 0.1) faceIndex = 5; // 顶面 (Y+) - 青色
    
    fragColor = faceColors[faceIndex];
}