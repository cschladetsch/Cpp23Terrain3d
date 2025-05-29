#include "Biome.h"
#include <algorithm>

const BiomeParams BiomeGenerator::DESERT_PARAMS = {
    25.0f,                                    // heightScale - rolling dunes
    0.2f,                                     // roughness - smooth dunes
    glm::vec3(0.96f, 0.87f, 0.5f),          // baseColor (bright sand)
    glm::vec3(0.85f, 0.65f, 0.35f),         // midColor (orange sand)
    glm::vec3(0.6f, 0.4f, 0.2f),            // peakColor (dark rock)
    15.0f,                                    // colorThreshold1
    25.0f                                     // colorThreshold2
};

const BiomeParams BiomeGenerator::FOREST_PARAMS = {
    40.0f,                                    // heightScale - rolling hills
    0.6f,                                     // roughness - varied terrain
    glm::vec3(0.15f, 0.5f, 0.1f),           // baseColor (lush grass)
    glm::vec3(0.08f, 0.3f, 0.05f),          // midColor (deep forest)
    glm::vec3(0.4f, 0.35f, 0.25f),          // peakColor (earthy brown)
    20.0f,                                    // colorThreshold1
    35.0f                                     // colorThreshold2
};

const BiomeParams BiomeGenerator::MOUNTAIN_PARAMS = {
    120.0f,                                   // heightScale - tall peaks
    0.9f,                                     // roughness - very rough
    glm::vec3(0.4f, 0.35f, 0.3f),           // baseColor (dark rock)
    glm::vec3(0.55f, 0.5f, 0.45f),          // midColor (granite)
    glm::vec3(1.0f, 1.0f, 1.0f),            // peakColor (pure snow)
    40.0f,                                    // colorThreshold1
    70.0f                                     // colorThreshold2
};

const BiomeParams BiomeGenerator::TUNDRA_PARAMS = {
    30.0f,                                    // heightScale - low hills
    0.5f,                                     // roughness - moderate
    glm::vec3(0.35f, 0.45f, 0.35f),         // baseColor (arctic grass)
    glm::vec3(0.75f, 0.8f, 0.85f),          // midColor (permafrost)
    glm::vec3(0.95f, 0.97f, 1.0f),          // peakColor (glacial ice)
    12.0f,                                    // colorThreshold1
    22.0f                                     // colorThreshold2
};

BiomeGenerator::BiomeGenerator(unsigned int seed) : biomeNoise(seed) {}

BiomeType BiomeGenerator::getBiome(float x, float z) const {
    float temperature = biomeNoise.octaveNoise(x * 0.002, z * 0.002, 0, 3, 0.5);
    float moisture = biomeNoise.octaveNoise(x * 0.002 + 1000, z * 0.002 + 1000, 0, 3, 0.5);
    
    if (temperature > 0.3f) {
        return moisture > 0 ? BiomeType::FOREST : BiomeType::DESERT;
    } else {
        return moisture > 0 ? BiomeType::TUNDRA : BiomeType::MOUNTAINS;
    }
}

BiomeParams BiomeGenerator::getBiomeParams(BiomeType type) const {
    switch (type) {
        case BiomeType::DESERT: return DESERT_PARAMS;
        case BiomeType::FOREST: return FOREST_PARAMS;
        case BiomeType::MOUNTAINS: return MOUNTAIN_PARAMS;
        case BiomeType::TUNDRA: return TUNDRA_PARAMS;
    }
    return FOREST_PARAMS;
}

float BiomeGenerator::getBiomeBlend(float x, float z, BiomeType& primary, BiomeType& secondary) const {
    const float blendDistance = 50.0f;
    
    primary = getBiome(x, z);
    
    BiomeType neighbors[4] = {
        getBiome(x + blendDistance, z),
        getBiome(x - blendDistance, z),
        getBiome(x, z + blendDistance),
        getBiome(x, z - blendDistance)
    };
    
    for (int i = 0; i < 4; ++i) {
        if (neighbors[i] != primary) {
            secondary = neighbors[i];
            
            float distance = blendDistance;
            for (float d = 0; d < blendDistance; d += 5.0f) {
                BiomeType test = getBiome(
                    x + (i == 0 ? d : (i == 1 ? -d : 0)),
                    z + (i == 2 ? d : (i == 3 ? -d : 0))
                );
                if (test != primary) {
                    distance = d;
                    break;
                }
            }
            
            return std::min(1.0f, distance / blendDistance);
        }
    }
    
    secondary = primary;
    return 1.0f;
}

float BiomeGenerator::generateHeight(float x, float z, const PerlinNoise& heightNoise) const {
    BiomeType primary, secondary;
    float blend = getBiomeBlend(x, z, primary, secondary);
    
    BiomeParams params1 = getBiomeParams(primary);
    BiomeParams params2 = getBiomeParams(secondary);
    
    // Multiple octaves for interesting terrain
    float scale1 = 0.01f;
    float scale2 = 0.005f;
    float scale3 = 0.002f;
    
    // Large scale features (continents)
    float continent1 = heightNoise.octaveNoise(x * scale3, z * scale3, 0, 2, 0.4f);
    float continent2 = heightNoise.octaveNoise(x * scale3, z * scale3, 100, 2, 0.4f);
    
    // Medium scale features (mountains/valleys)
    float medium1 = heightNoise.octaveNoise(x * scale2, z * scale2, 0, 4, params1.roughness);
    float medium2 = heightNoise.octaveNoise(x * scale2, z * scale2, 0, 4, params2.roughness);
    
    // Small scale details
    float detail1 = heightNoise.octaveNoise(x * scale1, z * scale1, 0, 6, params1.roughness);
    float detail2 = heightNoise.octaveNoise(x * scale1, z * scale1, 0, 6, params2.roughness);
    
    // Combine scales with different weights
    float baseHeight1 = continent1 * 0.5f + medium1 * 0.35f + detail1 * 0.15f;
    float baseHeight2 = continent2 * 0.5f + medium2 * 0.35f + detail2 * 0.15f;
    
    // Add interesting features based on biome
    if (primary == BiomeType::DESERT) {
        // Add dune-like ridges
        float dunes = std::sin(x * 0.05f) * std::sin(z * 0.05f) * 0.2f;
        baseHeight1 += dunes;
    } else if (primary == BiomeType::MOUNTAINS) {
        // Add sharp peaks
        float peaks = std::pow(std::abs(baseHeight1), 1.5f) * (baseHeight1 > 0 ? 1 : -1);
        baseHeight1 = peaks;
    }
    
    float height1 = baseHeight1 * params1.heightScale;
    float height2 = baseHeight2 * params2.heightScale;
    
    return height1 * blend + height2 * (1.0f - blend);
}

glm::vec3 BiomeGenerator::getColor(float x, float z, float height, const PerlinNoise& heightNoise) const {
    BiomeType primary, secondary;
    float blend = getBiomeBlend(x, z, primary, secondary);
    
    auto getColorForBiome = [&](BiomeType type) -> glm::vec3 {
        BiomeParams params = getBiomeParams(type);
        
        if (height < params.colorThreshold1) {
            return params.baseColor;
        } else if (height < params.colorThreshold2) {
            float t = (height - params.colorThreshold1) / (params.colorThreshold2 - params.colorThreshold1);
            return params.baseColor * (1.0f - t) + params.midColor * t;
        } else {
            float t = std::min(1.0f, (height - params.colorThreshold2) / params.colorThreshold2);
            return params.midColor * (1.0f - t) + params.peakColor * t;
        }
    };
    
    glm::vec3 color1 = getColorForBiome(primary);
    glm::vec3 color2 = getColorForBiome(secondary);
    
    return color1 * blend + color2 * (1.0f - blend);
}