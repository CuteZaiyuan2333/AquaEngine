#version 450

layout(location = 0) out vec3 fragColor;

// 立方体的8个顶点
vec3 positions[8] = vec3[](
    vec3(-1.0, -1.0, -1.0), // 0: 左下后
    vec3( 1.0, -1.0, -1.0), // 1: 右下后
    vec3( 1.0,  1.0, -1.0), // 2: 右上后
    vec3(-1.0,  1.0, -1.0), // 3: 左上后
    vec3(-1.0, -1.0,  1.0), // 4: 左下前
    vec3( 1.0, -1.0,  1.0), // 5: 右下前
    vec3( 1.0,  1.0,  1.0), // 6: 右上前
    vec3(-1.0,  1.0,  1.0)  // 7: 左上前
);

// 立方体的36个索引（12个三角形，每个面2个三角形）
int indices[36] = int[](
    // 后面 (z = -1.0)
    0, 1, 2,  2, 3, 0,
    // 前面 (z = 1.0)
    4, 5, 6,  6, 7, 4,
    // 左面 (x = -1.0)
    0, 3, 7,  7, 4, 0,
    // 右面 (x = 1.0)
    1, 2, 6,  6, 5, 1,
    // 下面 (y = -1.0)
    0, 1, 5,  5, 4, 0,
    // 上面 (y = 1.0)
    3, 2, 6,  6, 7, 3
);

// 6个面的颜色
vec3 faceColors[6] = vec3[](
    vec3(1.0, 0.0, 0.0), // 红色 - 后面
    vec3(0.0, 1.0, 0.0), // 绿色 - 前面
    vec3(0.0, 0.0, 1.0), // 蓝色 - 左面
    vec3(1.0, 1.0, 0.0), // 黄色 - 右面
    vec3(1.0, 0.0, 1.0), // 洋红 - 下面
    vec3(0.0, 1.0, 1.0)  // 青色 - 上面
);

void main() {
    int vertexIndex = indices[gl_VertexIndex];
    vec3 pos = positions[vertexIndex];
    
    // 创建一个简单的旋转动画
    // 使用顶点索引创建不同的旋转角度
    float angle = float(gl_VertexIndex) * 0.1;
    
    // 绕Y轴旋转
    float cosA = cos(angle);
    float sinA = sin(angle);
    
    vec3 rotated = vec3(
        pos.x * cosA - pos.z * sinA,
        pos.y,
        pos.x * sinA + pos.z * cosA
    );
    
    // 缩放并向后移动
    rotated = rotated * 0.3;
    rotated.z -= 6.0; // 向后移动很远
    
    // 简单的透视投影
    float perspective = 1.0 / (1.0 - rotated.z * 0.1);
    
    gl_Position = vec4(
        rotated.x * perspective,
        rotated.y * perspective,
        0.5, // 固定深度
        1.0
    );
    
    // 根据面来设置颜色
    int faceIndex = gl_VertexIndex / 6;
    fragColor = faceColors[faceIndex];
}