#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    // 采样纹理
    vec4 texColor = texture(texSampler, fragTexCoord);
    
    // 简化逻辑：如果fragColor是白色，显示纹理；否则显示顶点颜色
    if (fragColor.r > 0.9 && fragColor.g > 0.9 && fragColor.b > 0.9) {
        // 白色 - 显示纹理（右边立方体）
        outColor = vec4(texColor.rgb, 1.0);
    } else {
        // 其他颜色 - 显示顶点颜色（左边立方体）
        outColor = vec4(fragColor, 1.0);
    }
}