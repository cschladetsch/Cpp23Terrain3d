#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Perlin.h"
#include "Shader.h"

class Terrain {
private:
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    int width, height;
    float scale;
    std::unique_ptr<PerlinNoise> perlin;
    
    void generateTerrain();
    void setupMesh();
    
public:
    Terrain(int width, int height, float scale);
    ~Terrain();
    
    void render(const Shader& shader);
    float getHeight(float x, float z) const;
};