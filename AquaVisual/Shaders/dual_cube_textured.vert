#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform PushConstants {
    float time;
    float aspectRatio;
} pc;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

// 旋转矩阵函数
mat4 rotationY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        c,  0.0, s,  0.0,
        0.0, 1.0, 0.0, 0.0,
        -s, 0.0, c,  0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

mat4 rotationX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, c,  -s,  0.0,
        0.0, s,   c,  0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

void main() {
    vec3 localPos = inPosition;
    vec3 worldPos;
    
    // 根据X坐标判断是左边还是右边的立方体
    if (inPosition.x < 0.0) {
        // 左边立方体：先移到原点，旋转，再移回原位置
        vec3 cubeCenter = vec3(-2.0, 0.0, 0.0);
        vec3 relativePos = localPos - cubeCenter;
        
        // 绕Y轴旋转
        mat4 rotation = rotationY(pc.time);
        vec3 rotatedPos = (rotation * vec4(relativePos, 1.0)).xyz;
        worldPos = rotatedPos + cubeCenter;
        
        // 左边立方体：根据法线设置颜色
        fragColor = inColor;
    } else {
        // 右边立方体：先移到原点，旋转，再移回原位置
        vec3 cubeCenter = vec3(2.0, 0.0, 0.0);
        vec3 relativePos = localPos - cubeCenter;
        
        // 绕Y轴和X轴旋转
        mat4 rotationYMat = rotationY(pc.time);
        mat4 rotationXMat = rotationX(pc.time * 0.7);
        vec3 rotatedPos = (rotationYMat * rotationXMat * vec4(relativePos, 1.0)).xyz;
        worldPos = rotatedPos + cubeCenter;
        
        // 右边立方体：使用白色作为基色，让纹理显示
        fragColor = vec3(1.0, 1.0, 1.0);
    }
    
    fragTexCoord = inTexCoord;
    
    gl_Position = ubo.proj * ubo.view * vec4(worldPos, 1.0);
}