#pragma once

#include <unordered_map>
#include <memory>
#include <queue>
#include <glm/glm.hpp>
#include "TerrainChunk.h"
#include "Shader.h"
#include "Perlin.h"
#include "Biome.h"
#include "Config.h"

struct ChunkHash {
    std::size_t operator()(const glm::ivec2& k) const {
        return std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 1);
    }
};

class DynamicTerrain {
private:
    
    std::unordered_map<glm::ivec2, std::unique_ptr<TerrainChunk>, ChunkHash> chunks;
    std::queue<std::unique_ptr<TerrainChunk>> chunkPool;
    
    std::unique_ptr<PerlinNoise> heightNoise;
    std::unique_ptr<BiomeGenerator> biomeGen;
    
    glm::ivec2 lastPlayerChunk;
    
    void updateChunks(const glm::vec3& playerPos, const glm::mat4& viewProjection);
    int calculateLOD(float distance) const;
    int getNeighborLOD(const glm::ivec2& coord) const;
    std::unique_ptr<TerrainChunk> getOrCreateChunk(const glm::ivec2& coord);
    
public:
    DynamicTerrain();
    
    void update(const glm::vec3& playerPos, const glm::mat4& viewProjection);
    void render(Shader& shader, Shader& shadowShader, const glm::mat4& lightSpaceMatrix, const glm::mat4& viewProjection = glm::mat4(1.0f));
    float getHeightAt(float x, float z) const;
    glm::vec3 getColorAt(float x, float z, float height) const;
};