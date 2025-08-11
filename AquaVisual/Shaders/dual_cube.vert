#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(push_constant) uniform PushConstants {
    float time;
    float aspectRatio;
} pc;

layout(binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
} camera;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    // 创建两个立方体的位置
    vec3 leftCubePos = vec3(-3.0, 0.0, 0.0);   // 左边立方体
    vec3 rightCubePos = vec3(3.0, 0.0, 0.0);   // 右边立方体
    
    // 根据顶点索引决定使用哪个立方体的位置
    // 每个立方体有24个顶点（6面 * 4顶点）
    int cubeIndex = gl_VertexIndex / 24;
    vec3 cubePosition = (cubeIndex == 0) ? leftCubePos : rightCubePos;
    
    // 应用旋转（绕Y轴）
    float rotationAngle = pc.time;
    mat3 rotationMatrix = mat3(
        cos(rotationAngle), 0.0, sin(rotationAngle),
        0.0, 1.0, 0.0,
        -sin(rotationAngle), 0.0, cos(rotationAngle)
    );
    
    // 变换顶点位置
    vec3 rotatedPosition = rotationMatrix * inPosition;
    vec3 worldPosition = rotatedPosition + cubePosition;
    
    // 应用视图和投影变换
    gl_Position = camera.proj * camera.view * vec4(worldPosition, 1.0);
    
    // 根据法线设置颜色（不同面不同颜色）
    vec3 normal = normalize(inNormal);
    if (abs(normal.x) > 0.9) {
        fragColor = vec3(1.0, 0.0, 0.0); // 红色 - X面
    } else if (abs(normal.y) > 0.9) {
        fragColor = vec3(0.0, 1.0, 0.0); // 绿色 - Y面
    } else {
        fragColor = vec3(0.0, 0.0, 1.0); // 蓝色 - Z面
    }
    
    // 为右边的立方体添加不同的色调
    if (cubeIndex == 1) {
        fragColor = fragColor * 0.7 + vec3(0.3, 0.3, 0.0); // 添加黄色色调
    }
    
    fragTexCoord = inTexCoord;
}