#version 450

layout(push_constant) uniform PushConstants {
    float time;
} pc;

layout(location = 0) out vec3 fragColor;

vec3 positions[8] = vec3[](
    vec3(-0.5, -0.5, -0.5),  // 0
    vec3( 0.5, -0.5, -0.5),  // 1
    vec3( 0.5,  0.5, -0.5),  // 2
    vec3(-0.5,  0.5, -0.5),  // 3
    vec3(-0.5, -0.5,  0.5),  // 4
    vec3( 0.5, -0.5,  0.5),  // 5
    vec3( 0.5,  0.5,  0.5),  // 6
    vec3(-0.5,  0.5,  0.5)   // 7
);

int indices[36] = int[](
    // Front face
    0, 1, 2, 2, 3, 0,
    // Back face
    4, 6, 5, 6, 4, 7,
    // Left face
    4, 0, 3, 3, 7, 4,
    // Right face
    1, 5, 6, 6, 2, 1,
    // Top face
    3, 2, 6, 6, 7, 3,
    // Bottom face
    4, 5, 1, 1, 0, 4
);

vec3 colors[6] = vec3[](
    vec3(1.0, 0.0, 0.0),  // Red
    vec3(0.0, 1.0, 0.0),  // Green
    vec3(0.0, 0.0, 1.0),  // Blue
    vec3(1.0, 1.0, 0.0),  // Yellow
    vec3(1.0, 0.0, 1.0),  // Magenta
    vec3(0.0, 1.0, 1.0)   // Cyan
);

void main() {
    int vertexIndex = indices[gl_VertexIndex];
    vec3 pos = positions[vertexIndex];
    
    // 简单的旋转
    float angle = pc.time * 0.5;
    float cosA = cos(angle);
    float sinA = sin(angle);
    
    // Y轴旋转
    vec3 rotated = vec3(
        pos.x * cosA - pos.z * sinA,
        pos.y,
        pos.x * sinA + pos.z * cosA
    );
    
    // 移动到相机前方
    rotated.z -= 2.0;
    
    // 简单的透视投影
    float fov = 45.0 * 3.14159 / 180.0;
    float aspect = 800.0 / 600.0;
    float near = 0.1;
    float far = 100.0;
    
    float f = 1.0 / tan(fov / 2.0);
    
    gl_Position = vec4(
        rotated.x * f / aspect,
        rotated.y * f,
        (rotated.z * (far + near) - 2.0 * far * near) / (far - near),
        -rotated.z
    );
    
    // 根据面设置颜色
    int faceIndex = gl_VertexIndex / 6;
    fragColor = colors[faceIndex];
}