#version 450

// Input from vertex shader
layout(location = 0) in vec3 fragColor;

// Output color
layout(location = 0) out vec4 outColor;

void main() {
    // Simply output the interpolated color
    outColor = vec4(fragColor, 1.0);
}