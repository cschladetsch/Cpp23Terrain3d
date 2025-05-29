#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include "Shader.h"

class Water {
private:
    static constexpr int GRID_SIZE = 256;
    static constexpr float WAVE_SPEED = 0.03f;
    static constexpr float DAMPING = 0.999f;
    static constexpr float WATER_LEVEL = 0.0f;
    
    GLuint VAO, VBO, EBO;
    GLuint heightTexture[2];
    GLuint velocityTexture;
    GLuint normalTexture;
    GLuint FBO[2];
    GLuint reflectionFBO, reflectionTexture, reflectionDepth;
    GLuint refractionFBO, refractionTexture, refractionDepth;
    
    int currentBuffer = 0;
    float time = 0.0f;
    
    std::unique_ptr<Shader> waterShader;
    std::unique_ptr<Shader> waveUpdateShader;
    std::unique_ptr<Shader> normalComputeShader;
    
    void setupMesh();
    void setupFramebuffers(int windowWidth, int windowHeight);
    void setupComputeTextures();
    
public:
    Water(int windowWidth, int windowHeight);
    ~Water();
    
    void update(float deltaTime);
    void render(const glm::mat4& view, const glm::mat4& projection, 
                const glm::vec3& cameraPos, const glm::vec3& lightPos,
                GLuint terrainShadowMap, float fogDensity = 0.00008f, 
                float fogStart = 500.0f, const glm::vec3& fogColor = glm::vec3(0.02f, 0.02f, 0.08f));
    
    void beginReflectionPass();
    void endReflectionPass();
    void beginRefractionPass();
    void endRefractionPass();
    
    GLuint getReflectionTexture() const { return reflectionTexture; }
    GLuint getRefractionTexture() const { return refractionTexture; }
    
    void addRipple(float x, float z, float strength);
    float getWaterLevel() const { return WATER_LEVEL; }
};