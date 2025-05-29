#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;

void main() {
    // Vertices are already in world space
    gl_Position = lightSpaceMatrix * vec4(aPos, 1.0);
}