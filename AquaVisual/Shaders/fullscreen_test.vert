#version 450

layout(push_constant) uniform PushConstants {
    float time;
} pc;

layout(location = 0) out vec3 fragColor;

void main() {
    // 创建一个覆盖整个屏幕的大三角形
    vec2 positions[3] = vec2[](
        vec2(-3.0, -1.0),  // 左下角，超出屏幕
        vec2( 3.0, -1.0),  // 右下角，超出屏幕
        vec2( 0.0,  3.0)   // 顶部，超出屏幕
    );
    
    gl_Position = vec4(positions[gl_VertexIndex % 3], 0.0, 1.0);
    
    // 根据顶点索引设置不同颜色
    if (gl_VertexIndex % 3 == 0) {
        fragColor = vec3(1.0, 0.0, 0.0);  // 红色
    } else if (gl_VertexIndex % 3 == 1) {
        fragColor = vec3(0.0, 1.0, 0.0);  // 绿色
    } else {
        fragColor = vec3(0.0, 0.0, 1.0);  // 蓝色
    }
}