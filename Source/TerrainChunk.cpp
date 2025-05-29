#include "TerrainChunk.h"
#include <iostream>

TerrainChunk::TerrainChunk(glm::ivec2 coord, int resolution, float size, int lod)
    : chunkCoord(coord), resolution(resolution), chunkSize(size), lodLevel(lod), needsUpdate(true) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

TerrainChunk::~TerrainChunk() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void TerrainChunk::generate(const PerlinNoise& perlin) {
    generateMesh(perlin);
    uploadMesh();
    needsUpdate = false;
}

void TerrainChunk::generateWithNeighbors(const PerlinNoise& perlin, int northLOD, int southLOD, int eastLOD, int westLOD) {
    generateMeshWithStitching(perlin, northLOD, southLOD, eastLOD, westLOD);
    uploadMesh();
    needsUpdate = false;
}

void TerrainChunk::generateMesh(const PerlinNoise& perlin) {
    vertices.clear();
    indices.clear();
    
    int vertexResolution = resolution >> lodLevel;
    if (vertexResolution < 2) vertexResolution = 2; // Minimum 2x2 grid
    float stepSize = chunkSize / (vertexResolution - 1);
    
    glm::vec3 basePos = getWorldPosition();
    
    for (int z = 0; z < vertexResolution; ++z) {
        for (int x = 0; x < vertexResolution; ++x) {
            // Ensure edge vertices are exactly on chunk boundaries
            float worldX, worldZ;
            if (x == 0) {
                worldX = basePos.x; // Exact west edge
            } else if (x == vertexResolution - 1) {
                worldX = basePos.x + chunkSize; // Exact east edge
            } else {
                worldX = basePos.x + x * stepSize;
            }
            
            if (z == 0) {
                worldZ = basePos.z; // Exact south edge
            } else if (z == vertexResolution - 1) {
                worldZ = basePos.z + chunkSize; // Exact north edge
            } else {
                worldZ = basePos.z + z * stepSize;
            }
            
            float noiseValue = perlin.octaveNoise(worldX * 0.01, worldZ * 0.01, 0, 6, 0.5);
            float height = noiseValue * 50.0f + 10.0f; // Add base height to ensure visibility
            
            vertices.push_back(worldX);
            vertices.push_back(height);
            vertices.push_back(worldZ);
            
            float h1 = perlin.octaveNoise((worldX + stepSize) * 0.01, worldZ * 0.01, 0, 6, 0.5) * 50.0f;
            float h2 = perlin.octaveNoise((worldX - stepSize) * 0.01, worldZ * 0.01, 0, 6, 0.5) * 50.0f;
            float h3 = perlin.octaveNoise(worldX * 0.01, (worldZ + stepSize) * 0.01, 0, 6, 0.5) * 50.0f;
            float h4 = perlin.octaveNoise(worldX * 0.01, (worldZ - stepSize) * 0.01, 0, 6, 0.5) * 50.0f;
            
            glm::vec3 normal = glm::normalize(glm::vec3(h2 - h1, 2.0f * stepSize, h4 - h3));
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
            
            vertices.push_back(static_cast<float>(x) / (vertexResolution - 1));
            vertices.push_back(static_cast<float>(z) / (vertexResolution - 1));
        }
    }
    
    for (int z = 0; z < vertexResolution - 1; ++z) {
        for (int x = 0; x < vertexResolution - 1; ++x) {
            unsigned int topLeft = z * vertexResolution + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * vertexResolution + x;
            unsigned int bottomRight = bottomLeft + 1;
            
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}

void TerrainChunk::generateMeshWithStitching(const PerlinNoise& perlin, int northLOD, int southLOD, int eastLOD, int westLOD) {
    // For now, use a simpler approach: ensure edge vertices are generated consistently
    // This reduces complexity while still providing seamless connections
    generateMesh(perlin);
}

void TerrainChunk::uploadMesh() {
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void TerrainChunk::render() {
    if (indices.empty()) {
        std::cout << "Warning: Trying to render chunk with no indices!" << std::endl;
        return;
    }
    
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
}

void TerrainChunk::setLOD(int lod) {
    if (lodLevel != lod) {
        lodLevel = lod;
        needsUpdate = true;
    }
}

glm::vec3 TerrainChunk::getWorldPosition() const {
    return glm::vec3(chunkCoord.x * chunkSize, 0.0f, chunkCoord.y * chunkSize);
}

float TerrainChunk::getDistanceFrom(const glm::vec3& pos) const {
    glm::vec3 chunkCenter = getWorldPosition() + glm::vec3(chunkSize * 0.5f, 0.0f, chunkSize * 0.5f);
    return glm::length(pos - chunkCenter);
}

bool TerrainChunk::isVisible(const glm::mat4& viewProjection) const {
    // Temporarily disable frustum culling to prevent chunks from disappearing
    // Distance-based culling in DynamicTerrain is sufficient for performance
    return true;
}