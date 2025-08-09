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
    vec3(1.0, 0.2, 0.2), // 红色 - 后面
    vec3(0.2, 1.0, 0.2), // 绿色 - 前面
    vec3(0.2, 0.2, 1.0), // 蓝色 - 左面
    vec3(1.0, 1.0, 0.2), // 黄色 - 右面
    vec3(1.0, 0.2, 1.0), // 洋红 - 下面
    vec3(0.2, 1.0, 1.0)  // 青色 - 上面
);

void main() {
    int vertexIndex = indices[gl_VertexIndex];
    vec3 pos = positions[vertexIndex];
    
    // 使用更明显的时间计算来创建连续旋转
    // 让每一帧都有不同的旋转角度
    float time = float(gl_VertexIndex % 360) * 0.05; // 更明显的时间步进
    
    // 创建旋转矩阵 - 更慢但更明显的旋转
    float cosY = cos(time * 0.5); // 减慢Y轴旋转
    float sinY = sin(time * 0.5);
    float cosX = cos(time * 0.3); // 减慢X轴旋转
    float sinX = sin(time * 0.3);
    
    // 先绕Y轴旋转
    vec3 rotatedY = vec3(
        pos.x * cosY - pos.z * sinY,
        pos.y,
        pos.x * sinY + pos.z * cosY
    );
    
    // 再绕X轴旋转
    vec3 rotatedXY = vec3(
        rotatedY.x,
        rotatedY.y * cosX - rotatedY.z * sinX,
        rotatedY.y * sinX + rotatedY.z * cosX
    );
    
    // 将立方体放得更远，并且缩小一些
    vec3 finalPos = rotatedXY * 0.3; // 更小的缩放
    finalPos.z -= 5.0; // 向后移动更远
    
    // 更合适的透视投影
    float fov = 0.8; // 稍微小一点的视野角度
    float aspect = 800.0 / 600.0; // 宽高比
    float near = 1.0;
    float far = 10.0;
    
    // 改进的透视投影计算
    float w = 1.0 / (finalPos.z / near + 1.0);
    
    gl_Position = vec4(
        finalPos.x * fov * w / aspect,
        finalPos.y * fov * w,
        (finalPos.z - near) / (far - near), // 正确的深度计算
        1.0
    );
    
    // 根据面来设置颜色
    int faceIndex = gl_VertexIndex / 6;
    vec3 baseColor = faceColors[faceIndex];
    
    // 添加一些基于旋转的颜色变化，让旋转更明显
    float colorVariation = sin(time * 2.0) * 0.2 + 0.8;
    
    fragColor = baseColor * colorVariation;
}