#include "Config.h"
#include <fstream>
#include <iostream>

Config* Config::instance = nullptr;

Config& Config::getInstance() {
    if (instance == nullptr) {
        instance = new Config();
    }
    return *instance;
}

glm::vec3 Config::parseVec3(const json& arr) {
    return glm::vec3(arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>());
}

void Config::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + filename);
    }
    
    file >> configData;
    file.close();
    
    // Parse window config
    auto& w = configData["window"];
    window.width = w["width"];
    window.height = w["height"];
    window.title = w["title"];
    window.fullscreen = w["fullscreen"];
    window.vsync = w["vsync"];
    
    // Parse rendering config
    auto& r = configData["rendering"];
    rendering.shadowMapResolution = r["shadowMapResolution"];
    rendering.waterReflectionScale = r["waterReflectionScale"];
    rendering.fieldOfView = r["fieldOfView"];
    rendering.nearPlane = r["nearPlane"];
    rendering.farPlane = r["farPlane"];
    rendering.enableShadows = r["enableShadows"];
    rendering.enableWater = r["enableWater"];
    rendering.enableSkybox = r["enableSkybox"];
    
    // Parse camera config
    auto& c = configData["camera"];
    camera.initialPosition = parseVec3(c["initialPosition"]);
    camera.initialYaw = c["initialYaw"];
    camera.initialPitch = c["initialPitch"];
    camera.acceleration = c["acceleration"];
    camera.maxSpeed = c["maxSpeed"];
    camera.reverseSpeedFactor = c["reverseSpeedFactor"];
    camera.mouseSensitivity = c["mouseSensitivity"];
    camera.turnSpeed = c["turnSpeed"];
    camera.airResistance = c["airResistance"];
    camera.minHeightAboveTerrain = c["minHeightAboveTerrain"];
    
    // Parse terrain config
    auto& t = configData["terrain"];
    terrain.chunkSize = t["chunkSize"];
    terrain.chunkResolution = t["chunkResolution"];
    terrain.viewDistance = t["viewDistance"];
    terrain.maxLodLevels = t["maxLodLevels"];
    terrain.lodDistances = t["lodDistances"].get<std::vector<float>>();
    terrain.heightNoiseSeed = t["heightNoiseSeed"];
    terrain.biomeNoiseSeed = t["biomeNoiseSeed"];
    terrain.maxChunkPoolSize = t["maxChunkPoolSize"];
    
    // Parse biomes
    auto& b = configData["biomes"];
    for (auto& [name, biomeData] : b.items()) {
        BiomeConfig biome;
        biome.heightScale = biomeData["heightScale"];
        biome.roughness = biomeData["roughness"];
        biome.baseColor = parseVec3(biomeData["baseColor"]);
        biome.midColor = parseVec3(biomeData["midColor"]);
        biome.peakColor = parseVec3(biomeData["peakColor"]);
        biome.colorThreshold1 = biomeData["colorThreshold1"];
        biome.colorThreshold2 = biomeData["colorThreshold2"];
        biomes[name] = biome;
    }
    
    // Parse water config
    auto& wa = configData["water"];
    water.level = wa["level"];
    water.gridSize = wa["gridSize"];
    water.meshDivisions = wa["meshDivisions"];
    water.waveSpeed = wa["waveSpeed"];
    water.damping = wa["damping"];
    water.color = parseVec3(wa["color"]);
    water.deepColor = parseVec3(wa["deepColor"]);
    water.transparency = wa["transparency"];
    water.reflectionDistortion = wa["reflectionDistortion"];
    water.specularPower = wa["specularPower"];
    
    // Parse lighting config
    auto& l = configData["lighting"];
    lighting.sunOffset = parseVec3(l["sunOffset"]);
    lighting.ambientStrength = l["ambientStrength"];
    lighting.shadowBias = l["shadowBias"];
    lighting.shadowNearPlane = l["shadowNearPlane"];
    lighting.shadowFarPlane = l["shadowFarPlane"];
    lighting.shadowOrthoSize = l["shadowOrthoSize"];
    
    // Parse Perlin noise config
    auto& pn = configData["perlinNoise"];
    perlinNoise.terrainScales.continental = pn["terrainScales"]["continental"];
    perlinNoise.terrainScales.regional = pn["terrainScales"]["regional"];
    perlinNoise.terrainScales.local = pn["terrainScales"]["local"];
    
    perlinNoise.octaves.continental = pn["octaves"]["continental"];
    perlinNoise.octaves.regional = pn["octaves"]["regional"];
    perlinNoise.octaves.local = pn["octaves"]["local"];
    
    perlinNoise.persistence.continental = pn["persistence"]["continental"];
    perlinNoise.persistence.regional = pn["persistence"]["regional"];
    perlinNoise.persistence.local = pn["persistence"]["local"];
    
    perlinNoise.weights.continental = pn["weights"]["continental"];
    perlinNoise.weights.regional = pn["weights"]["regional"];
    perlinNoise.weights.local = pn["weights"]["local"];
    
    // Parse performance config
    auto& p = configData["performance"];
    performance.targetFPS = p["targetFPS"];
    performance.enableVsync = p["enableVsync"];
    performance.multiThreadedChunkGeneration = p["multiThreadedChunkGeneration"];
    performance.chunkGenerationThreads = p["chunkGenerationThreads"];
    
    std::cout << "Config loaded successfully from: " << filename << std::endl;
}

void Config::saveToFile(const std::string& filename) {
    // Update JSON data with current values
    configData["window"]["width"] = window.width;
    configData["window"]["height"] = window.height;
    configData["window"]["title"] = window.title;
    configData["window"]["fullscreen"] = window.fullscreen;
    configData["window"]["vsync"] = window.vsync;
    
    // Save to file
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to save config file: " + filename);
    }
    
    file << std::setw(2) << configData << std::endl;
    file.close();
    
    std::cout << "Config saved successfully to: " << filename << std::endl;
}