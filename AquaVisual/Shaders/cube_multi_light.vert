#version 450

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragWorldPos;

// Push constants for time-based animation and aspect ratio
layout(push_constant) uniform PushConstants {
    float time;
    float aspectRatio;
} pc;

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

// 每个面的法线
vec3 faceNormals[6] = vec3[](
    vec3(0.0, 0.0, -1.0), // 后面
    vec3(0.0, 0.0,  1.0), // 前面
    vec3(-1.0, 0.0, 0.0), // 左面
    vec3( 1.0, 0.0, 0.0), // 右面
    vec3(0.0, -1.0, 0.0), // 下面
    vec3(0.0,  1.0, 0.0)  // 上面
);

// 6个面的基础颜色 - 全部改为白色以便观察光照效果
vec3 faceColors[6] = vec3[](
    vec3(1.0, 1.0, 1.0), // 白色 - 后面
    vec3(1.0, 1.0, 1.0), // 白色 - 前面
    vec3(1.0, 1.0, 1.0), // 白色 - 左面
    vec3(1.0, 1.0, 1.0), // 白色 - 右面
    vec3(1.0, 1.0, 1.0), // 白色 - 下面
    vec3(1.0, 1.0, 1.0)  // 白色 - 上面
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

// 创建视图矩阵（简单的相机向后移动）
mat4 lookAt(vec3 eye, vec3 center, vec3 up) {
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    
    return mat4(
        s.x, u.x, -f.x, 0.0,
        s.y, u.y, -f.y, 0.0,
        s.z, u.z, -f.z, 0.0,
        -dot(s, eye), -dot(u, eye), dot(f, eye), 1.0
    );
}

void main() {
    int vertexIndex = indices[gl_VertexIndex];
    vec3 pos = positions[vertexIndex];
    
    // 使用push constant中的时间
    float time = pc.time;
    
    // 创建模型变换矩阵 - 立方体在原点附近
    mat4 model = translate(vec3(0.0, 0.0, 0.0)) *  // 立方体在原点
                 rotateY(time * 1.0) *              // 绕Y轴旋转
                 rotateX(time * 0.7) *              // 绕X轴旋转
                 mat4(2.0, 0.0, 0.0, 0.0,          // 放大2倍，更容易看见
                      0.0, 2.0, 0.0, 0.0,
                      0.0, 0.0, 2.0, 0.0,
                      0.0, 0.0, 0.0, 1.0);
    
    // 创建视图矩阵 - 相机位置在(0, 0, 5)，看向原点
    mat4 view = lookAt(vec3(0.0, 0.0, 5.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
    
    // 创建透视投影矩阵 - 限制宽高比范围，避免极端拉伸
    float aspectRatio = clamp(pc.aspectRatio, 0.5, 3.0); // 限制在0.5到3.0之间
    mat4 proj = perspective(radians(60.0), aspectRatio, 0.1, 100.0);
    
    // 应用完整的MVP变换
    vec4 worldPos = model * vec4(pos, 1.0);
    gl_Position = proj * view * worldPos;
    
    // 传递世界坐标位置用于光照计算
    fragWorldPos = worldPos.xyz;
    
    // 计算法线（需要考虑模型变换）
    int faceIndex = gl_VertexIndex / 6;
    vec3 normal = faceNormals[faceIndex];
    // 应用模型变换到法线（只考虑旋转，不考虑平移）
    mat3 normalMatrix = mat3(model);
    fragNormal = normalize(normalMatrix * normal);
    
    // 设置基础颜色
    fragColor = faceColors[faceIndex];
}