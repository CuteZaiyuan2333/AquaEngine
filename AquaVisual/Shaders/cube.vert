#version 450

layout(push_constant) uniform PushConstants {
    float time;
} pc;

layout(location = 0) out vec3 fragColor;

// Cube vertices in 3D space
vec3 positions[8] = vec3[](
    vec3(-0.5, -0.5, -0.5), // 0
    vec3( 0.5, -0.5, -0.5), // 1
    vec3( 0.5,  0.5, -0.5), // 2
    vec3(-0.5,  0.5, -0.5), // 3
    vec3(-0.5, -0.5,  0.5), // 4
    vec3( 0.5, -0.5,  0.5), // 5
    vec3( 0.5,  0.5,  0.5), // 6
    vec3(-0.5,  0.5,  0.5)  // 7
);

// Cube indices for 12 triangles
int indices[36] = int[](
    // Front face
    0, 1, 2, 2, 3, 0,
    // Back face  
    4, 6, 5, 6, 4, 7,
    // Left face
    4, 0, 3, 3, 7, 4,
    // Right face
    1, 5, 6, 6, 2, 1,
    // Bottom face
    4, 5, 1, 1, 0, 4,
    // Top face
    3, 2, 6, 6, 7, 3
);

// Face colors
vec3 colors[6] = vec3[](
    vec3(1.0, 0.0, 0.0), // Front - Red
    vec3(0.0, 1.0, 0.0), // Back - Green
    vec3(0.0, 0.0, 1.0), // Left - Blue
    vec3(1.0, 1.0, 0.0), // Right - Yellow
    vec3(1.0, 0.0, 1.0), // Bottom - Magenta
    vec3(0.0, 1.0, 1.0)  // Top - Cyan
);

mat4 perspective(float fovy, float aspect, float near, float far) {
    float f = 1.0 / tan(fovy / 2.0);
    return mat4(
        f / aspect, 0.0, 0.0, 0.0,
        0.0, f, 0.0, 0.0,
        0.0, 0.0, (far + near) / (near - far), (2.0 * far * near) / (near - far),
        0.0, 0.0, -1.0, 0.0
    );
}

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
    
    // Apply transformations
    float time = pc.time;
    mat4 model = translate(vec3(0.0, 0.0, -3.0)) * 
                 rotateY(time * 0.8) * 
                 rotateX(time * 0.3);
    
    // Proper perspective projection
    mat4 proj = perspective(radians(45.0), 800.0/600.0, 0.1, 100.0);
    
    // Transform vertex
    vec4 worldPos = model * vec4(pos, 1.0);
    gl_Position = proj * worldPos;
    
    // Set color based on face
    int faceIndex = gl_VertexIndex / 6;
    fragColor = colors[faceIndex];
}