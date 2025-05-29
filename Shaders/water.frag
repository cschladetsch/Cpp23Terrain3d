#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec4 ClipSpace;
in vec3 ToCameraVector;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D shadowMap;
uniform sampler2D normalTexture;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform float time;

const vec3 waterColor = vec3(0.0, 0.3, 0.5);
const vec3 deepColor = vec3(0.0, 0.1, 0.2);

vec3 calculateNormal() {
    float offset = 0.01;
    vec3 normal = vec3(0.0, 1.0, 0.0);
    
    // Add wave-based normal perturbation
    normal.x = sin(FragPos.x * 0.1 + time * 2.0) * 0.1;
    normal.z = sin(FragPos.z * 0.15 + time * 1.5) * 0.1;
    
    return normalize(normal);
}

void main() {
    vec2 ndc = (ClipSpace.xy / ClipSpace.w) * 0.5 + 0.5;
    vec2 reflectTexCoords = vec2(ndc.x, 1.0 - ndc.y);
    vec2 refractTexCoords = ndc;
    
    // Add distortion based on waves
    float distortionStrength = 0.02;
    vec3 normal = calculateNormal();
    reflectTexCoords += normal.xz * distortionStrength;
    refractTexCoords += normal.xz * distortionStrength;
    
    reflectTexCoords = clamp(reflectTexCoords, 0.001, 0.999);
    refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);
    
    vec3 reflectColor = texture(reflectionTexture, reflectTexCoords).rgb;
    vec3 refractColor = texture(refractionTexture, refractTexCoords).rgb;
    
    // Fresnel effect
    vec3 viewVector = normalize(ToCameraVector);
    float refractiveFactor = dot(viewVector, vec3(0.0, 1.0, 0.0));
    refractiveFactor = pow(refractiveFactor, 0.5);
    
    // Mix colors
    vec3 color = mix(reflectColor, refractColor, refractiveFactor);
    color = mix(color, waterColor, 0.2);
    
    // Add specular highlights
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewVector);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
    color += vec3(1.0) * spec;
    
    // More opaque water
    float depth = gl_FragCoord.z;
    float alpha = clamp(depth * 10.0, 0.7, 0.95);
    
    FragColor = vec4(color, alpha);
}