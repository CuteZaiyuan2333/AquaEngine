#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragWorldPos;
layout(location = 3) out vec3 fragColor;

void main() {
    vec4 worldPos = ubo.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;
    
    fragWorldPos = worldPos.xyz;
    fragNormal = mat3(ubo.model) * inNormal;
    fragTexCoord = inTexCoord;
    
    // Generate colors based on face normals
    vec3 normal = normalize(inNormal);
    if (abs(normal.z) > 0.9) {
        // Front/Back faces
        fragColor = normal.z > 0.0 ? vec3(1.0, 0.2, 0.2) : vec3(0.2, 1.0, 0.2); // Red/Green
    } else if (abs(normal.x) > 0.9) {
        // Left/Right faces  
        fragColor = normal.x > 0.0 ? vec3(1.0, 1.0, 0.2) : vec3(0.2, 0.2, 1.0); // Yellow/Blue
    } else {
        // Top/Bottom faces
        fragColor = normal.y > 0.0 ? vec3(1.0, 0.2, 1.0) : vec3(0.2, 1.0, 1.0); // Magenta/Cyan
    }
}