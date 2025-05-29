#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Perlin.h"

class TerrainChunk {
private:
    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    glm::ivec2 chunkCoord;
    int resolution;
    float chunkSize;
    int lodLevel;
    bool needsUpdate;
    
    void generateMesh(const PerlinNoise& perlin);
    void generateMeshWithStitching(const PerlinNoise& perlin, int northLOD, int southLOD, int eastLOD, int westLOD);
    void uploadMesh();
    
public:
    TerrainChunk(glm::ivec2 coord, int resolution, float size, int lod = 0);
    ~TerrainChunk();
    
    void generate(const PerlinNoise& perlin);
    void generateWithNeighbors(const PerlinNoise& perlin, int northLOD, int southLOD, int eastLOD, int westLOD);
    void render();
    void setLOD(int lod);
    int getLOD() const { return lodLevel; }
    
    glm::ivec2 getCoord() const { return chunkCoord; }
    glm::vec3 getWorldPosition() const;
    float getDistanceFrom(const glm::vec3& pos) const;
    bool isVisible(const glm::mat4& viewProjection) const;
};