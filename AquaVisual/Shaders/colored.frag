#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragWorldPos;
layout(location = 3) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    // Simple diffuse lighting
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    vec3 normal = normalize(fragNormal);
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Ambient lighting
    float ambient = 0.4;
    
    // Combine lighting with vertex color
    vec3 lighting = vec3(ambient + diff * 0.6);
    vec3 finalColor = fragColor * lighting;
    
    outColor = vec4(finalColor, 1.0);
}