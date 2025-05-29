#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>
#include "Shader.h"

class Skybox {
private:
    GLuint VAO, VBO;
    GLuint textureID;
    std::unique_ptr<Shader> skyboxShader;
    
    void loadCubemap(const std::vector<std::string>& faces);
    void setupMesh();
    
public:
    Skybox();
    ~Skybox();
    
    void render(const glm::mat4& view, const glm::mat4& projection);
    GLuint getTextureID() const { return textureID; }
};