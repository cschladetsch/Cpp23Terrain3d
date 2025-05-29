#include "DynamicTerrain.h"
#include <algorithm>
#include <execution>
#include <iostream>
#include <climits>
#include <glm/gtc/matrix_transform.hpp>

DynamicTerrain::DynamicTerrain() {
    Config& config = Config::getInstance();
    heightNoise = std::make_unique<PerlinNoise>(config.terrain.heightNoiseSeed);
    biomeGen = std::make_unique<BiomeGenerator>(config.terrain.biomeNoiseSeed);
    lastPlayerChunk = glm::ivec2(INT_MAX, INT_MAX);
    
    // Pre-allocate chunk pool
    for (int i = 0; i < config.terrain.maxChunkPoolSize; ++i) {
        chunkPool.push(std::make_unique<TerrainChunk>(glm::ivec2(0, 0), config.terrain.chunkResolution, config.terrain.chunkSize));
    }
}

void DynamicTerrain::update(const glm::vec3& playerPos, const glm::mat4& viewProjection) {
    updateChunks(playerPos, viewProjection);
}

void DynamicTerrain::updateChunks(const glm::vec3& playerPos, const glm::mat4& viewProjection) {
    Config& config = Config::getInstance();
    glm::ivec2 playerChunk(
        static_cast<int>(std::floor(playerPos.x / config.terrain.chunkSize)),
        static_cast<int>(std::floor(playerPos.z / config.terrain.chunkSize))
    );
    
    // Force update on first frame
    bool forceUpdate = (lastPlayerChunk.x == INT_MAX);
    
    // Only update if player moved to new chunk
    if (!forceUpdate && playerChunk == lastPlayerChunk) {
        return;
    }
    lastPlayerChunk = playerChunk;
    
    // Mark chunks for removal if too far
    std::vector<glm::ivec2> toRemove;
    for (auto& [coord, chunk] : chunks) {
        if (std::abs(coord.x - playerChunk.x) > config.terrain.viewDistance ||
            std::abs(coord.y - playerChunk.y) > config.terrain.viewDistance) {
            toRemove.push_back(coord);
        }
    }
    
    // Remove and recycle distant chunks
    for (const auto& coord : toRemove) {
        auto it = chunks.find(coord);
        if (it != chunks.end()) {
            chunkPool.push(std::move(it->second));
            chunks.erase(it);
        }
    }
    
    // Generate new chunks around player
    for (int z = -config.terrain.viewDistance; z <= config.terrain.viewDistance; ++z) {
        for (int x = -config.terrain.viewDistance; x <= config.terrain.viewDistance; ++x) {
            glm::ivec2 coord = playerChunk + glm::ivec2(x, z);
            
            if (chunks.find(coord) == chunks.end()) {
                auto chunk = getOrCreateChunk(coord);
                
                // Calculate LOD based on distance
                float distance = chunk->getDistanceFrom(playerPos);
                int lod = calculateLOD(distance);
                chunk->setLOD(lod);
                
                // Generate terrain with biome support (initial generation)
                chunk->generate(*heightNoise);
                chunks[coord] = std::move(chunk);
            }
        }
    }
    
    
    // Update LOD levels for existing chunks
    for (auto& [coord, chunk] : chunks) {
        float distance = chunk->getDistanceFrom(playerPos);
        int newLod = calculateLOD(distance);
        if (chunk->getLOD() != newLod) {
            chunk->setLOD(newLod);
            chunk->generate(*heightNoise); // Regenerate if LOD changed
        }
    }
    
    // Note: Stitching system temporarily disabled for debugging
    // The edge alignment fix should reduce gaps significantly
}

int DynamicTerrain::calculateLOD(float distance) const {
    Config& config = Config::getInstance();
    for (int i = 0; i < config.terrain.lodDistances.size(); ++i) {
        if (distance < config.terrain.lodDistances[i]) {
            return i;
        }
    }
    return config.terrain.maxLodLevels - 1;
}

int DynamicTerrain::getNeighborLOD(const glm::ivec2& coord) const {
    auto it = chunks.find(coord);
    if (it != chunks.end()) {
        return it->second->getLOD();
    }
    return -1; // No neighbor found
}

std::unique_ptr<TerrainChunk> DynamicTerrain::getOrCreateChunk(const glm::ivec2& coord) {
    Config& config = Config::getInstance();
    if (!chunkPool.empty()) {
        auto chunk = std::move(chunkPool.front());
        chunkPool.pop();
        // Reinitialize with new coordinates
        chunk = std::make_unique<TerrainChunk>(coord, config.terrain.chunkResolution, config.terrain.chunkSize);
        return chunk;
    }
    return std::make_unique<TerrainChunk>(coord, config.terrain.chunkResolution, config.terrain.chunkSize);
}

void DynamicTerrain::render(Shader& shader, Shader& shadowShader, const glm::mat4& lightSpaceMatrix, const glm::mat4& viewProjection) {
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    
    // Render chunks with frustum culling for infinite terrain
    int renderedChunks = 0;
    int totalChunks = chunks.size();
    
    for (auto& [coord, chunk] : chunks) {
        // Use frustum culling to only render visible chunks
        if (chunk->isVisible(viewProjection)) {
            chunk->render();
            renderedChunks++;
        }
    }
}

float DynamicTerrain::getHeightAt(float x, float z) const {
    return biomeGen->generateHeight(x, z, *heightNoise);
}

glm::vec3 DynamicTerrain::getColorAt(float x, float z, float height) const {
    return biomeGen->getColor(x, z, height, *heightNoise);
}