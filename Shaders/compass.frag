#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform float heading; // In degrees

void main() {
    vec2 center = vec2(0.5, 0.5);
    vec2 pos = TexCoord - center;
    float dist = length(pos);
    float radius = 0.45;
    
    if (dist > radius) {
        discard;
    }
    
    // Compass background
    vec3 color = vec3(0.05, 0.05, 0.15);
    float alpha = 0.9;
    
    // Outer ring
    if (dist > radius * 0.9) {
        color = vec3(0.8, 0.8, 0.8);
        alpha = 1.0;
    }
    
    // Inner ring
    if (dist > radius * 0.85 && dist < radius * 0.9) {
        color = vec3(0.2, 0.2, 0.3);
        alpha = 1.0;
    }
    
    // Calculate angle from center (0° = North/Up, clockwise)
    float angle = atan(pos.x, pos.y) * 180.0 / 3.14159;
    angle = mod(angle + 360.0, 360.0);
    
    // Major tick marks every 30 degrees
    float tickAngle = mod(angle, 30.0);
    if (tickAngle < 3.0 || tickAngle > 27.0) {
        if (dist > radius * 0.65 && dist < radius * 0.85) {
            color = vec3(0.9, 0.9, 0.9);
            alpha = 1.0;
        }
    }
    
    // Minor tick marks every 10 degrees
    float minorTickAngle = mod(angle, 10.0);
    if (minorTickAngle < 1.5 || minorTickAngle > 8.5) {
        if (dist > radius * 0.7 && dist < radius * 0.8) {
            color = vec3(0.6, 0.6, 0.6);
            alpha = 1.0;
        }
    }
    
    // Cardinal directions
    // North (0°) - Red
    if (abs(angle) < 8.0 || abs(angle - 360.0) < 8.0) {
        if (dist > radius * 0.5 && dist < radius * 0.7) {
            color = vec3(1.0, 0.3, 0.3);
            alpha = 1.0;
        }
    }
    
    // East (90°) - White
    if (abs(angle - 90.0) < 8.0) {
        if (dist > radius * 0.5 && dist < radius * 0.7) {
            color = vec3(0.9, 0.9, 0.9);
            alpha = 1.0;
        }
    }
    
    // South (180°) - White
    if (abs(angle - 180.0) < 8.0) {
        if (dist > radius * 0.5 && dist < radius * 0.7) {
            color = vec3(0.9, 0.9, 0.9);
            alpha = 1.0;
        }
    }
    
    // West (270°) - White
    if (abs(angle - 270.0) < 8.0) {
        if (dist > radius * 0.5 && dist < radius * 0.7) {
            color = vec3(0.9, 0.9, 0.9);
            alpha = 1.0;
        }
    }
    
    // Heading needle (aircraft heading)
    float needleAngle = heading * 3.14159 / 180.0;
    vec2 needleDir = vec2(sin(needleAngle), cos(needleAngle)); // Note: swapped for correct orientation
    float needleDot = dot(normalize(pos), needleDir);
    
    // Main needle
    if (needleDot > 0.9 && dist < radius * 0.7) {
        color = vec3(1.0, 1.0, 0.0); // Yellow needle
        alpha = 1.0;
    }
    
    // Needle tail (opposite direction)
    if (needleDot < -0.9 && dist < radius * 0.3) {
        color = vec3(1.0, 1.0, 0.0);
        alpha = 1.0;
    }
    
    // Center dot
    if (dist < radius * 0.08) {
        color = vec3(1.0, 1.0, 0.0);
        alpha = 1.0;
    }
    
    FragColor = vec4(color, alpha);
}