#include "Terrain.h"
#include <iostream>

Terrain::Terrain(int width, int height, float scale) 
    : width(width), height(height), scale(scale) {
    perlin = std::make_unique<PerlinNoise>(42);
    generateTerrain();
    setupMesh();
}

Terrain::~Terrain() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Terrain::generateTerrain() {
    vertices.clear();
    indices.clear();
    
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    for (int z = 0; z <= height; ++z) {
        for (int x = 0; x <= width; ++x) {
            float xPos = (x - halfWidth) * scale / width;
            float zPos = (z - halfHeight) * scale / height;
            
            float noiseValue = perlin->octaveNoise(x * 0.02, z * 0.02, 0, 4, 0.5);
            float yPos = noiseValue * 30.0f;
            
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
            
            float nx = perlin->octaveNoise((x + 1) * 0.02, z * 0.02, 0, 4, 0.5) - 
                      perlin->octaveNoise((x - 1) * 0.02, z * 0.02, 0, 4, 0.5);
            float nz = perlin->octaveNoise(x * 0.02, (z + 1) * 0.02, 0, 4, 0.5) - 
                      perlin->octaveNoise(x * 0.02, (z - 1) * 0.02, 0, 4, 0.5);
            
            glm::vec3 normal = glm::normalize(glm::vec3(-nx * 30.0f, 2.0f, -nz * 30.0f));
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
            
            vertices.push_back(static_cast<float>(x) / width);
            vertices.push_back(static_cast<float>(z) / height);
        }
    }
    
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            unsigned int topLeft = z * (width + 1) + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * (width + 1) + x;
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

void Terrain::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void Terrain::render(const Shader& shader) {
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

float Terrain::getHeight(float x, float z) const {
    float noiseX = (x / scale + 0.5f) * width;
    float noiseZ = (z / scale + 0.5f) * height;
    return perlin->octaveNoise(noiseX * 0.02, noiseZ * 0.02, 0, 4, 0.5) * 30.0f;
}