#include "Water.h"
#include <vector>
#include <iostream>

Water::Water(int windowWidth, int windowHeight) {
    setupMesh();
    setupFramebuffers(windowWidth, windowHeight);
    setupComputeTextures();
    
    waterShader = std::make_unique<Shader>("Shaders/water.vert", "Shaders/water.frag");
    // Compute shaders would be implemented for more advanced water simulation
    // For now, we'll use simple vertex-based waves
}

Water::~Water() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(2, heightTexture);
    glDeleteTextures(1, &velocityTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteFramebuffers(2, FBO);
    glDeleteFramebuffers(1, &reflectionFBO);
    glDeleteFramebuffers(1, &refractionFBO);
    glDeleteTextures(1, &reflectionTexture);
    glDeleteTextures(1, &refractionTexture);
    glDeleteRenderbuffers(1, &reflectionDepth);
    glDeleteRenderbuffers(1, &refractionDepth);
}

void Water::setupMesh() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    float size = 1000.0f;
    int divisions = 100;
    float step = size / divisions;
    
    for (int z = 0; z <= divisions; ++z) {
        for (int x = 0; x <= divisions; ++x) {
            float xPos = -size/2 + x * step;
            float zPos = -size/2 + z * step;
            
            vertices.push_back(xPos);
            vertices.push_back(WATER_LEVEL);
            vertices.push_back(zPos);
            
            vertices.push_back(static_cast<float>(x) / divisions);
            vertices.push_back(static_cast<float>(z) / divisions);
        }
    }
    
    for (int z = 0; z < divisions; ++z) {
        for (int x = 0; x < divisions; ++x) {
            unsigned int topLeft = z * (divisions + 1) + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * (divisions + 1) + x;
            unsigned int bottomRight = bottomLeft + 1;
            
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void Water::setupFramebuffers(int windowWidth, int windowHeight) {
    glGenFramebuffers(1, &reflectionFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
    
    glGenTextures(1, &reflectionTexture);
    glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth/2, windowHeight/2, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);
    
    glGenRenderbuffers(1, &reflectionDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, reflectionDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth/2, windowHeight/2);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, reflectionDepth);
    
    glGenFramebuffers(1, &refractionFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
    
    glGenTextures(1, &refractionTexture);
    glBindTexture(GL_TEXTURE_2D, refractionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth/2, windowHeight/2, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTexture, 0);
    
    glGenRenderbuffers(1, &refractionDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, refractionDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth/2, windowHeight/2);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, refractionDepth);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Water::setupComputeTextures() {
    glGenTextures(2, heightTexture);
    for (int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, heightTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, GRID_SIZE, GRID_SIZE, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    
    glGenTextures(1, &velocityTexture);
    glBindTexture(GL_TEXTURE_2D, velocityTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, GRID_SIZE, GRID_SIZE, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, GRID_SIZE, GRID_SIZE, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Water::update(float deltaTime) {
    time += deltaTime;
}

void Water::render(const glm::mat4& view, const glm::mat4& projection,
                   const glm::vec3& cameraPos, const glm::vec3& lightPos,
                   GLuint terrainShadowMap) {
    waterShader->use();
    waterShader->setMat4("view", view);
    waterShader->setMat4("projection", projection);
    waterShader->setVec3("cameraPos", cameraPos);
    waterShader->setVec3("lightPos", lightPos);
    waterShader->setFloat("time", time);
    waterShader->setInt("reflectionTexture", 0);
    waterShader->setInt("refractionTexture", 1);
    waterShader->setInt("shadowMap", 2);
    waterShader->setInt("normalTexture", 3);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, refractionTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, terrainShadowMap);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 100 * 100 * 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    glDisable(GL_BLEND);
}

void Water::beginReflectionPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
    glViewport(0, 0, 640, 360);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Water::endReflectionPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Water::beginRefractionPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
    glViewport(0, 0, 640, 360);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Water::endRefractionPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Water::addRipple(float x, float z, float strength) {
    // Would implement compute shader dispatch to add ripple at position
}
