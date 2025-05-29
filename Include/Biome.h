#pragma once

#include <glm/glm.hpp>
#include "Perlin.h"

enum class BiomeType {
    DESERT,
    FOREST,
    MOUNTAINS,
    TUNDRA
};

struct BiomeParams {
    float heightScale;
    float roughness;
    glm::vec3 baseColor;
    glm::vec3 midColor;
    glm::vec3 peakColor;
    float colorThreshold1;
    float colorThreshold2;
};

class BiomeGenerator {
private:
    PerlinNoise biomeNoise;
    
    static const BiomeParams DESERT_PARAMS;
    static const BiomeParams FOREST_PARAMS;
    static const BiomeParams MOUNTAIN_PARAMS;
    static const BiomeParams TUNDRA_PARAMS;
    
public:
    BiomeGenerator(unsigned int seed = 12345);
    
    BiomeType getBiome(float x, float z) const;
    BiomeParams getBiomeParams(BiomeType type) const;
    float getBiomeBlend(float x, float z, BiomeType& primary, BiomeType& secondary) const;
    
    float generateHeight(float x, float z, const PerlinNoise& heightNoise) const;
    glm::vec3 getColor(float x, float z, float height, const PerlinNoise& heightNoise) const;
};