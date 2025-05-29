#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
in vec3 worldPos;

uniform float time;
uniform vec3 sunDirection;

// Melbourne coordinates: -37.8136° S, 144.9631° E
const float MELBOURNE_LAT = -37.8136;
const float MELBOURNE_LON = 144.9631;

// Major stars visible from Melbourne, Australia (Southern Hemisphere)
const int NUM_STARS = 25;
const vec3 starPositions[NUM_STARS] = vec3[](
    // Brightest stars visible from Melbourne
    vec3(0.3, 0.8, 0.5),    // Sirius (brightest star)
    vec3(-0.5, -0.6, 0.6),  // Canopus (2nd brightest)
    vec3(0.1, -0.9, 0.4),   // Alpha Centauri A
    vec3(0.15, -0.85, 0.5), // Alpha Centauri B (nearby)
    vec3(0.8, 0.2, 0.6),    // Arcturus
    vec3(-0.3, 0.7, 0.7),   // Vega
    vec3(0.9, 0.4, 0.2),    // Capella
    vec3(-0.6, 0.8, 0.0),   // Rigel
    vec3(0.2, -0.3, -0.9),  // Achernar
    vec3(-0.7, 0.7, -0.1),  // Betelgeuse
    vec3(0.4, -0.7, 0.6),   // Hadar (Beta Centauri)
    vec3(0.7, 0.6, 0.4),    // Altair
    vec3(-0.4, 0.9, 0.2),   // Aldebaran
    vec3(0.0, -0.4, 0.9),   // Spica
    vec3(-0.8, -0.2, 0.6),  // Antares
    vec3(0.6, 0.8, 0.0),    // Pollux
    vec3(0.9, -0.4, 0.2),   // Fomalhaut
    vec3(-0.2, 0.2, 0.95),  // Deneb
    // Southern Cross (Crux) constellation
    vec3(0.2, -0.8, 0.6),   // Acrux (Alpha Crucis)
    vec3(0.0, -0.7, 0.7),   // Gacrux (Gamma Crucis)
    vec3(0.3, -0.7, 0.6),   // Becrux (Beta Crucis)
    vec3(0.1, -0.6, 0.8),   // Delta Crucis
    // Additional southern stars
    vec3(-0.3, -0.8, 0.5),  // Shaula
    vec3(0.8, -0.6, 0.1),   // Peacock
    vec3(-0.6, -0.3, 0.7)   // Adhara
);

const float starMagnitudes[NUM_STARS] = float[](
    -1.46, -0.74, -0.27, -0.01, -0.05, 0.03, 0.08, 0.13, 0.46, 0.50,
    0.61, 0.77, 0.85, 1.04, 1.09, 1.14, 1.16, 1.25, 0.77, 1.63,
    1.25, 2.80, 1.62, 1.89, 1.85
);

vec3 calculateSkyColor(vec3 direction) {
    float elevation = direction.y;
    
    // Always nighttime - dark sky for Melbourne stargazing
    vec3 nightZenith = vec3(0.01, 0.01, 0.05);   // Very dark blue at zenith
    vec3 nightHorizon = vec3(0.05, 0.03, 0.08);  // Slightly lighter at horizon
    
    // Add subtle gradient from horizon to zenith
    float elevationFactor = clamp(elevation, 0.0, 1.0);
    vec3 skyColor = mix(nightHorizon, nightZenith, elevationFactor);
    
    return skyColor;
}

float starVisibility(float magnitude, float skyBrightness) {
    // Dark sky conditions - all stars should be visible
    float limitingMag = 6.5; // Perfect dark sky limiting magnitude
    return clamp((limitingMag - magnitude) / 3.0, 0.0, 1.0);
}

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i + vec2(0.0, 0.0)), hash(i + vec2(1.0, 0.0)), u.x),
               mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), u.x), u.y);
}

vec3 addStars(vec3 direction, vec3 skyColor) {
    vec3 finalColor = skyColor;
    
    for(int i = 0; i < NUM_STARS; i++) {
        vec3 starDir = normalize(starPositions[i]);
        float distance = length(direction - starDir);
        
        // Star size based on magnitude (brighter = larger)
        float magnitude = starMagnitudes[i];
        float baseStarSize = 0.03 / (1.0 + magnitude * 0.4);
        
        // Twinkling/scintillation effect
        float twinkleSpeed = 3.0 + magnitude * 0.5;
        float twinkle = 0.7 + 0.3 * sin(time * twinkleSpeed + float(i) * 12.34);
        
        // Atmospheric scintillation (stars near horizon twinkle more)
        float elevation = starDir.y;
        float atmosphericTwinkle = 1.0 - 0.3 * exp(-elevation * 3.0);
        twinkle *= atmosphericTwinkle;
        
        float starSize = baseStarSize * twinkle;
        float visibility = starVisibility(magnitude, 0.0);
        
        if(distance < starSize && visibility > 0.0) {
            // Distance-based intensity with smooth falloff
            float intensity = pow(1.0 - distance / starSize, 2.0) * visibility;
            
            // Color temperature based on star type (simplified)
            vec3 starColor;
            if(magnitude < -0.5) {
                starColor = vec3(0.9, 0.9, 1.2); // Blue-white giants
            } else if(magnitude < 0.5) {
                starColor = vec3(1.0, 1.0, 1.0); // White stars
            } else {
                starColor = vec3(1.2, 1.0, 0.8); // Yellow-orange stars
            }
            
            // Add diffraction spikes for bright stars
            if(magnitude < 1.0) {
                vec2 spikeDir = normalize(direction.xz - starDir.xz);
                float spike1 = exp(-abs(dot(spikeDir, vec2(1, 0))) * 100.0) * 0.3;
                float spike2 = exp(-abs(dot(spikeDir, vec2(0, 1))) * 100.0) * 0.3;
                intensity += (spike1 + spike2) * twinkle;
            }
            
            // Atmospheric glow for very bright stars
            if(magnitude < 0.0) {
                float glow = exp(-distance * 50.0) * 0.1 * twinkle;
                intensity += glow;
            }
            
            finalColor += starColor * intensity * twinkle * 1.5;
        }
    }
    
    // Add faint background star field
    vec2 starField = direction.xz * 50.0;
    float backgroundStars = noise(starField) * noise(starField * 2.0) * noise(starField * 4.0);
    backgroundStars = pow(backgroundStars, 8.0) * 0.2;
    finalColor += vec3(0.8, 0.85, 1.0) * backgroundStars;
    
    return finalColor;
}

void main() {
    vec3 direction = normalize(TexCoords);
    
    // Calculate sun direction (simplified - would normally use time/date)
    vec3 sunDir = normalize(vec3(0.5, sin(time * 0.1), 0.3));
    
    vec3 skyColor = calculateSkyColor(direction);
    vec3 finalColor = addStars(direction, skyColor);
    
    FragColor = vec4(finalColor, 1.0);
}