#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec4 ClipSpace;
out vec3 ToCameraVector;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;
uniform float time;

void main() {
    vec3 pos = aPos;
    
    // Simple wave animation
    float wave1 = sin(pos.x * 0.1 + time * 2.0) * 0.5;
    float wave2 = sin(pos.z * 0.15 + time * 1.5) * 0.3;
    pos.y += (wave1 + wave2) * 0.5;
    
    FragPos = pos;
    TexCoords = aTexCoords;
    ToCameraVector = cameraPos - pos;
    
    ClipSpace = projection * view * vec4(pos, 1.0);
    gl_Position = ClipSpace;
}