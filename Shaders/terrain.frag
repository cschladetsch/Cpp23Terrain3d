#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D shadowMap;
uniform vec3 biomeColor;
uniform float fogDensity;
uniform float fogStart;
uniform vec3 fogColor;

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}

void main() {
    // Use height-based coloring mixed with biome color
    float height = FragPos.y / 50.0;
    vec3 heightColor = mix(vec3(0.2, 0.5, 0.2), vec3(0.8, 0.8, 0.8), height);
    vec3 color = mix(heightColor, biomeColor, 0.3);
    
    // Calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);
    
    // Simple lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * color;
    
    vec3 ambient = 0.3 * color;
    vec3 result = ambient + (1.0 - shadow) * diffuse;
    
    // Calculate fog based on distance from camera
    float distance = length(FragPos - viewPos);
    float fogFactor = 1.0 - exp(-fogDensity * max(0.0, distance - fogStart));
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    // Blend result with fog color (skybox color)
    result = mix(result, fogColor, fogFactor);
    
    FragColor = vec4(result, 1.0);
}