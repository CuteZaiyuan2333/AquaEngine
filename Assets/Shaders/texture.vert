#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) out vec2 fragTexCoord;

vec2 positions[6] = vec2[](
    // First triangle (bottom-left, top-left, top-right)
    vec2(-0.5, -0.5),
    vec2(-0.5,  0.5),
    vec2( 0.5,  0.5),
    // Second triangle (bottom-left, top-right, bottom-right)
    vec2(-0.5, -0.5),
    vec2( 0.5,  0.5),
    vec2( 0.5, -0.5)
);

vec2 texCoords[6] = vec2[](
    // First triangle
    vec2(0.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    // Second triangle
    vec2(0.0, 1.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0)
);

void main() {
    vec4 worldPos = ubo.model * vec4(positions[gl_VertexIndex], 0.0, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;
    fragTexCoord = texCoords[gl_VertexIndex];
}