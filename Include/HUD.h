#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include "Shader.h"

class HUD {
private:
    std::unique_ptr<Shader> compassShader;
    std::unique_ptr<Shader> uiShader;
    
    GLuint compassVAO, compassVBO;
    GLuint quadVAO, quadVBO;
    
    int windowWidth, windowHeight;
    
    void setupCompass();
    void setupQuad();
    void renderHeadingText(float heading, float pitch, float speed);
    void renderHeadingBar(float heading, float x, float y, float width, float height);
    
public:
    HUD(int width, int height);
    ~HUD();
    
    void render(float heading, float pitch, float speed);
    void updateWindowSize(int width, int height);
    
    // Convert yaw to compass heading (0-360 degrees, 0 = North)
    static float yawToHeading(float yaw);
};