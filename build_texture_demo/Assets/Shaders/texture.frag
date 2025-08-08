#version 450

layout(location = 0) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 texColor = texture(texSampler, fragTexCoord);
    
    // Add a slight border effect to make the texture more visible
    vec2 border = step(0.05, fragTexCoord) * step(0.05, 1.0 - fragTexCoord);
    float borderFactor = border.x * border.y;
    
    // Mix texture color with a slight white border
    outColor = mix(vec4(1.0, 1.0, 1.0, 1.0), texColor, borderFactor);
}