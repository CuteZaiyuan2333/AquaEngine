#version 450

// Push constants for time
layout(push_constant) uniform PushConstants {
    float time;
} pc;

layout(location = 0) out vec3 fragColor;

// Cube vertices (8 vertices)
vec3 cubeVertices[8] = vec3[](
    vec3(-1.0, -1.0, -1.0), // 0: left-bottom-back
    vec3( 1.0, -1.0, -1.0), // 1: right-bottom-back
    vec3( 1.0,  1.0, -1.0), // 2: right-top-back
    vec3(-1.0,  1.0, -1.0), // 3: left-top-back
    vec3(-1.0, -1.0,  1.0), // 4: left-bottom-front
    vec3( 1.0, -1.0,  1.0), // 5: right-bottom-front
    vec3( 1.0,  1.0,  1.0), // 6: right-top-front
    vec3(-1.0,  1.0,  1.0)  // 7: left-top-front
);

// Cube indices for 12 triangles (36 vertices)
int cubeIndices[36] = int[](
    // Back face
    0, 1, 2, 2, 3, 0,
    // Front face
    4, 5, 6, 6, 7, 4,
    // Left face
    0, 4, 7, 7, 3, 0,
    // Right face
    1, 5, 6, 6, 2, 1,
    // Bottom face
    0, 1, 5, 5, 4, 0,
    // Top face
    3, 2, 6, 6, 7, 3
);

// Face colors
vec3 faceColors[6] = vec3[](
    vec3(1.0, 0.0, 0.0), // Back: Red
    vec3(0.0, 1.0, 0.0), // Front: Green
    vec3(0.0, 0.0, 1.0), // Left: Blue
    vec3(1.0, 1.0, 0.0), // Right: Yellow
    vec3(1.0, 0.0, 1.0), // Bottom: Magenta
    vec3(0.0, 1.0, 1.0)  // Top: Cyan
);

void main() {
    // Get vertex index and face
    int vertexIndex = gl_VertexIndex;
    int faceIndex = vertexIndex / 6;
    int cubeVertexIndex = cubeIndices[vertexIndex];
    
    // Get the vertex position
    vec3 pos = cubeVertices[cubeVertexIndex];
    
    // Scale down the cube
    pos *= 0.3;
    
    // Simple rotation using time
    float rotY = pc.time * 0.5; // Slow rotation around Y axis
    float rotX = pc.time * 0.3; // Slow rotation around X axis
    
    // Apply Y rotation
    float cosY = cos(rotY);
    float sinY = sin(rotY);
    vec3 rotatedY = vec3(
        pos.x * cosY - pos.z * sinY,
        pos.y,
        pos.x * sinY + pos.z * cosY
    );
    
    // Apply X rotation
    float cosX = cos(rotX);
    float sinX = sin(rotX);
    vec3 rotatedPos = vec3(
        rotatedY.x,
        rotatedY.y * cosX - rotatedY.z * sinX,
        rotatedY.y * sinX + rotatedY.z * cosX
    );
    
    // Move cube away from camera
    rotatedPos.z -= 2.0;
    
    // Very simple perspective projection
    gl_Position = vec4(rotatedPos.x / (-rotatedPos.z * 0.5), 
                       rotatedPos.y / (-rotatedPos.z * 0.5), 
                       0.5, 1.0);
    
    // Set color based on face
    fragColor = faceColors[faceIndex];
}