#pragma once

#include <string>
#include <glm/glm.hpp>
#include "Json.hpp"

using json = nlohmann::json;

struct WindowConfig {
    int width;
    int height;
    std::string title;
    bool fullscreen;
    bool vsync;
};

struct RenderingConfig {
    int shadowMapResolution;
    float waterReflectionScale;
    float fieldOfView;
    float nearPlane;
    float farPlane;
    bool enableShadows;
    bool enableWater;
    bool enableSkybox;
};

struct CameraConfig {
    glm::vec3 initialPosition;
    float initialYaw;
    float initialPitch;
    float acceleration;
    float maxSpeed;
    float reverseSpeedFactor;
    float mouseSensitivity;
    float turnSpeed;
    float airResistance;
    float minHeightAboveTerrain;
};

struct TerrainConfig {
    int chunkSize;
    int chunkResolution;
    int viewDistance;
    int maxLodLevels;
    std::vector<float> lodDistances;
    unsigned int heightNoiseSeed;
    unsigned int biomeNoiseSeed;
    int maxChunkPoolSize;
};

struct BiomeConfig {
    float heightScale;
    float roughness;
    glm::vec3 baseColor;
    glm::vec3 midColor;
    glm::vec3 peakColor;
    float colorThreshold1;
    float colorThreshold2;
};

struct WaterConfig {
    float level;
    int gridSize;
    int meshDivisions;
    float waveSpeed;
    float damping;
    glm::vec3 color;
    glm::vec3 deepColor;
    float transparency;
    float reflectionDistortion;
    float specularPower;
};

struct LightingConfig {
    glm::vec3 sunOffset;
    float ambientStrength;
    float shadowBias;
    float shadowNearPlane;
    float shadowFarPlane;
    float shadowOrthoSize;
};

struct NoiseScales {
    float continental;
    float regional;
    float local;
};

struct NoiseOctaves {
    int continental;
    int regional;
    int local;
};

struct NoisePersistence {
    float continental;
    float regional;
    float local;
};

struct NoiseWeights {
    float continental;
    float regional;
    float local;
};

struct PerlinNoiseConfig {
    NoiseScales terrainScales;
    NoiseOctaves octaves;
    NoisePersistence persistence;
    NoiseWeights weights;
};

struct PerformanceConfig {
    int targetFPS;
    bool enableVsync;
    bool multiThreadedChunkGeneration;
    int chunkGenerationThreads;
};

class Config {
private:
    static Config* instance;
    json configData;
    
    Config() = default;
    
    glm::vec3 parseVec3(const json& arr);
    
public:
    WindowConfig window;
    RenderingConfig rendering;
    CameraConfig camera;
    TerrainConfig terrain;
    std::map<std::string, BiomeConfig> biomes;
    WaterConfig water;
    LightingConfig lighting;
    PerlinNoiseConfig perlinNoise;
    PerformanceConfig performance;
    
    static Config& getInstance();
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename);
    
    // Delete copy constructor and assignment operator
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
};