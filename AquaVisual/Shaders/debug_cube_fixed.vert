#version 450

layout(push_constant) uniform PushConstants {
    float time;
} pc;

layout(location = 0) out vec3 fragColor;

// 直接定义36个顶点（每个面6个顶点，共6个面）
vec3 vertices[36] = vec3[](
    // Front face (红色)
    vec3(-0.5, -0.5,  0.5), vec3( 0.5, -0.5,  0.5), vec3( 0.5,  0.5,  0.5),
    vec3( 0.5,  0.5,  0.5), vec3(-0.5,  0.5,  0.5), vec3(-0.5, -0.5,  0.5),
    
    // Back face (绿色)
    vec3(-0.5, -0.5, -0.5), vec3(-0.5,  0.5, -0.5), vec3( 0.5,  0.5, -0.5),
    vec3( 0.5,  0.5, -0.5), vec3( 0.5, -0.5, -0.5), vec3(-0.5, -0.5, -0.5),
    
    // Left face (蓝色)
    vec3(-0.5,  0.5,  0.5), vec3(-0.5,  0.5, -0.5), vec3(-0.5, -0.5, -0.5),
    vec3(-0.5, -0.5, -0.5), vec3(-0.5, -0.5,  0.5), vec3(-0.5,  0.5,  0.5),
    
    // Right face (黄色)
    vec3( 0.5,  0.5,  0.5), vec3( 0.5, -0.5, -0.5), vec3( 0.5,  0.5, -0.5),
    vec3( 0.5, -0.5, -0.5), vec3( 0.5,  0.5,  0.5), vec3( 0.5, -0.5,  0.5),
    
    // Top face (洋红色)
    vec3(-0.5,  0.5, -0.5), vec3(-0.5,  0.5,  0.5), vec3( 0.5,  0.5,  0.5),
    vec3( 0.5,  0.5,  0.5), vec3( 0.5,  0.5, -0.5), vec3(-0.5,  0.5, -0.5),
    
    // Bottom face (青色)
    vec3(-0.5, -0.5, -0.5), vec3( 0.5, -0.5, -0.5), vec3( 0.5, -0.5,  0.5),
    vec3( 0.5, -0.5,  0.5), vec3(-0.5, -0.5,  0.5), vec3(-0.5, -0.5, -0.5)
);

vec3 colors[6] = vec3[](
    vec3(1.0, 0.0, 0.0),  // 红色
    vec3(0.0, 1.0, 0.0),  // 绿色
    vec3(0.0, 0.0, 1.0),  // 蓝色
    vec3(1.0, 1.0, 0.0),  // 黄色
    vec3(1.0, 0.0, 1.0),  // 洋红色
    vec3(0.0, 1.0, 1.0)   // 青色
);

void main() {
    vec3 pos = vertices[gl_VertexIndex];
    
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