#version 450

layout(push_constant) uniform PushConstants {
    float time;
} pc;

layout(location = 0) out vec3 fragColor;

// 立方体的36个顶点（每个面6个顶点）
vec3 cubeVertices[36] = vec3[](
    // 前面 (Z = 0.5)
    vec3(-0.3, -0.3,  0.5), vec3( 0.3, -0.3,  0.5), vec3( 0.3,  0.3,  0.5),
    vec3( 0.3,  0.3,  0.5), vec3(-0.3,  0.3,  0.5), vec3(-0.3, -0.3,  0.5),
    
    // 后面 (Z = -0.5)
    vec3(-0.3, -0.3, -0.5), vec3(-0.3,  0.3, -0.5), vec3( 0.3,  0.3, -0.5),
    vec3( 0.3,  0.3, -0.5), vec3( 0.3, -0.3, -0.5), vec3(-0.3, -0.3, -0.5),
    
    // 左面 (X = -0.5)
    vec3(-0.3,  0.3,  0.5), vec3(-0.3,  0.3, -0.5), vec3(-0.3, -0.3, -0.5),
    vec3(-0.3, -0.3, -0.5), vec3(-0.3, -0.3,  0.5), vec3(-0.3,  0.3,  0.5),
    
    // 右面 (X = 0.5)
    vec3( 0.3,  0.3,  0.5), vec3( 0.3, -0.3,  0.5), vec3( 0.3, -0.3, -0.5),
    vec3( 0.3, -0.3, -0.5), vec3( 0.3,  0.3, -0.5), vec3( 0.3,  0.3,  0.5),
    
    // 底面 (Y = -0.5)
    vec3(-0.3, -0.3, -0.5), vec3( 0.3, -0.3, -0.5), vec3( 0.3, -0.3,  0.5),
    vec3( 0.3, -0.3,  0.5), vec3(-0.3, -0.3,  0.5), vec3(-0.3, -0.3, -0.5),
    
    // 顶面 (Y = 0.5)
    vec3(-0.3,  0.3, -0.5), vec3(-0.3,  0.3,  0.5), vec3( 0.3,  0.3,  0.5),
    vec3( 0.3,  0.3,  0.5), vec3( 0.3,  0.3, -0.5), vec3(-0.3,  0.3, -0.5)
);

// 每个面的颜色
vec3 faceColors[6] = vec3[](
    vec3(1.0, 0.0, 0.0), // 前面 - 红色
    vec3(0.0, 1.0, 0.0), // 后面 - 绿色
    vec3(0.0, 0.0, 1.0), // 左面 - 蓝色
    vec3(1.0, 1.0, 0.0), // 右面 - 黄色
    vec3(1.0, 0.0, 1.0), // 底面 - 紫色
    vec3(0.0, 1.0, 1.0)  // 顶面 - 青色
);

void main() {
    vec3 pos = cubeVertices[gl_VertexIndex];
    
    // 旋转矩阵 (绕Y轴旋转)
    float angle = pc.time * 0.5; // 减慢旋转速度
    mat3 rotY = mat3(
        cos(angle), 0.0, sin(angle),
        0.0,        1.0, 0.0,
        -sin(angle), 0.0, cos(angle)
    );
    
    // 应用旋转
    pos = rotY * pos;
    
    // 移动到相机前方
    pos.z -= 1.5;
    
    // 简单的透视投影
    float fov = 1.0; // 视野角度
    float aspect = 784.0 / 561.0; // 窗口宽高比
    float near = 0.1;
    float far = 10.0;
    
    // 透视除法
    vec4 projected = vec4(pos.x / (aspect * fov), pos.y / fov, pos.z, 1.0);
    projected.z = (projected.z - near) / (far - near);
    projected.xy /= -pos.z; // 透视除法
    
    gl_Position = projected;
    
    // 根据面设置颜色
    int faceIndex = gl_VertexIndex / 6;
    fragColor = faceColors[faceIndex];
}