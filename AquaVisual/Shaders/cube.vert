#version 450

layout(location = 0) out vec3 fragColor;

// 立方体的8个顶点
vec3 positions[8] = vec3[](
    vec3(-0.5, -0.5, -0.5), // 0: 左下后
    vec3( 0.5, -0.5, -0.5), // 1: 右下后
    vec3( 0.5,  0.5, -0.5), // 2: 右上后
    vec3(-0.5,  0.5, -0.5), // 3: 左上后
    vec3(-0.5, -0.5,  0.5), // 4: 左下前
    vec3( 0.5, -0.5,  0.5), // 5: 右下前
    vec3( 0.5,  0.5,  0.5), // 6: 右上前
    vec3(-0.5,  0.5,  0.5)  // 7: 左上前
);

// 立方体的36个索引（12个三角形，每个面2个三角形）
int indices[36] = int[](
    // 后面 (z = -0.5)
    0, 1, 2,  2, 3, 0,
    // 前面 (z = 0.5)
    4, 5, 6,  6, 7, 4,
    // 左面 (x = -0.5)
    0, 3, 7,  7, 4, 0,
    // 右面 (x = 0.5)
    1, 2, 6,  6, 5, 1,
    // 下面 (y = -0.5)
    0, 1, 5,  5, 4, 0,
    // 上面 (y = 0.5)
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

// 创建透视投影矩阵
mat4 perspective(float fovy, float aspect, float near, float far) {
    float f = 1.0 / tan(fovy / 2.0);
    return mat4(
        f / aspect, 0.0, 0.0, 0.0,
        0.0, f, 0.0, 0.0,
        0.0, 0.0, (far + near) / (near - far), (2.0 * far * near) / (near - far),
        0.0, 0.0, -1.0, 0.0
    );
}

// 创建旋转矩阵
mat4 rotateY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        c, 0.0, s, 0.0,
        0.0, 1.0, 0.0, 0.0,
        -s, 0.0, c, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

mat4 rotateX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, c, -s, 0.0,
        0.0, s, c, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

// 创建平移矩阵
mat4 translate(vec3 t) {
    return mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        t.x, t.y, t.z, 1.0
    );
}

void main() {
    int vertexIndex = indices[gl_VertexIndex];
    vec3 pos = positions[vertexIndex];
    
    // 使用一个简单的时间模拟来创建旋转动画
    // 这里使用一个基于顶点索引的简单时间计算
    float time = float(gl_VertexIndex % 1000) * 0.01;
    
    // 创建变换矩阵
    mat4 model = translate(vec3(0.0, 0.0, -3.0)) * // 向后移动
                 rotateY(time * 1.0) *              // 绕Y轴旋转
                 rotateX(time * 0.7);               // 绕X轴旋转
    
    // 创建透视投影矩阵
    mat4 proj = perspective(radians(45.0), 800.0/600.0, 0.1, 100.0);
    
    // 应用变换
    vec4 worldPos = model * vec4(pos, 1.0);
    gl_Position = proj * worldPos;
    
    // 根据面来设置颜色，添加一些变化
    int faceIndex = gl_VertexIndex / 6;
    vec3 baseColor = faceColors[faceIndex];
    
    // 添加一些基于时间的颜色变化
    float colorMod = sin(time * 3.0) * 0.3 + 0.7;
    fragColor = baseColor * colorMod;
}