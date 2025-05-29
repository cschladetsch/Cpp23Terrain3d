#include "HUD.h"
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <iomanip>

HUD::HUD(int width, int height) : windowWidth(width), windowHeight(height) {
    compassShader = std::make_unique<Shader>("Shaders/ui.vert", "Shaders/compass.frag");
    uiShader = std::make_unique<Shader>("Shaders/ui.vert", "Shaders/ui.frag");
    
    setupCompass();
    setupQuad();
}

HUD::~HUD() {
    glDeleteVertexArrays(1, &compassVAO);
    glDeleteBuffers(1, &compassVBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}

void HUD::setupCompass() {
    // Create a circle for the compass
    const int segments = 64;
    std::vector<float> vertices;
    
    // Center vertex
    vertices.push_back(0.0f); vertices.push_back(0.0f); // position
    vertices.push_back(0.5f); vertices.push_back(0.5f); // tex coord
    
    // Circle vertices
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.14159f * i / segments;
        float x = cos(angle);
        float y = sin(angle);
        
        vertices.push_back(x); vertices.push_back(y); // position
        vertices.push_back((x + 1.0f) * 0.5f); vertices.push_back((y + 1.0f) * 0.5f); // tex coord
    }
    
    glGenVertexArrays(1, &compassVAO);
    glGenBuffers(1, &compassVBO);
    
    glBindVertexArray(compassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, compassVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void HUD::setupQuad() {
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void HUD::render(float heading, float pitch, float speed) {
    // Enable blending for UI elements
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    
    // Set up orthographic projection for UI
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1.0f);
    
    // Render compass in top-right corner
    compassShader->use();
    
    // Position compass in top-right corner
    float compassSize = 100.0f;
    float compassX = windowWidth - compassSize - 20.0f;
    float compassY = windowHeight - compassSize - 20.0f;
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(compassX, compassY, 0.0f));
    model = glm::scale(model, glm::vec3(compassSize, compassSize, 1.0f));
    
    // Create projection matrix that maps [-1,1] to compass position
    glm::mat4 compassProjection = projection * model;
    compassShader->setMat4("projection", compassProjection);
    compassShader->setFloat("heading", heading);
    
    glBindVertexArray(compassVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 66); // 1 center + 65 circle vertices
    
    // Render numerical heading display
    renderHeadingText(heading, pitch, speed);
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void HUD::renderHeadingText(float heading, float pitch, float speed) {
    uiShader->use();
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1.0f);
    uiShader->setMat4("projection", projection);
    
    // Main info panel background
    float bgWidth = 220.0f;
    float bgHeight = 100.0f;
    float bgX = 20.0f;
    float bgY = windowHeight - bgHeight - 20.0f;
    
    // Background panel
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(bgX, bgY, 0.0f));
    model = glm::scale(model, glm::vec3(bgWidth, bgHeight, 1.0f));
    
    glm::mat4 bgProjection = projection * model;
    uiShader->setMat4("projection", bgProjection);
    uiShader->setVec3("color", glm::vec3(0.0f, 0.0f, 0.0f));
    uiShader->setFloat("alpha", 0.7f);
    
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Border
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(bgX - 2, bgY - 2, 0.0f));
    model = glm::scale(model, glm::vec3(bgWidth + 4, bgHeight + 4, 1.0f));
    bgProjection = projection * model;
    uiShader->setMat4("projection", bgProjection);
    uiShader->setVec3("color", glm::vec3(0.3f, 0.3f, 0.4f));
    uiShader->setFloat("alpha", 0.8f);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Redraw background over border
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(bgX, bgY, 0.0f));
    model = glm::scale(model, glm::vec3(bgWidth, bgHeight, 1.0f));
    bgProjection = projection * model;
    uiShader->setMat4("projection", bgProjection);
    uiShader->setVec3("color", glm::vec3(0.0f, 0.0f, 0.0f));
    uiShader->setFloat("alpha", 0.7f);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Visual heading bar (simple representation)
    renderHeadingBar(heading, bgX + 10, bgY + 20, bgWidth - 20, 15);
    
    // Note: For actual numerical text rendering, you would need to integrate a text rendering library
    // The heading value is: heading degrees, pitch: pitch degrees, speed: speed km/h
    // The compass provides the graphical heading indicator
}

void HUD::renderHeadingBar(float heading, float x, float y, float width, float height) {
    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1.0f);
    uiShader->setMat4("projection", projection);
    
    // Background bar
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    
    glm::mat4 barProjection = projection * model;
    uiShader->setMat4("projection", barProjection);
    uiShader->setVec3("color", glm::vec3(0.2f, 0.2f, 0.3f));
    uiShader->setFloat("alpha", 0.8f);
    
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Cardinal direction markers on the bar
    float cardinalWidth = 2.0f;
    float cardinalHeight = height * 0.6f;
    
    // North (0°)
    float northPos = (heading / 360.0f) * width;
    if (northPos >= 0 && northPos <= width) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + northPos - cardinalWidth/2, y + height*0.2f, 0.0f));
        model = glm::scale(model, glm::vec3(cardinalWidth, cardinalHeight, 1.0f));
        barProjection = projection * model;
        uiShader->setMat4("projection", barProjection);
        uiShader->setVec3("color", glm::vec3(1.0f, 0.3f, 0.3f)); // Red for North
        uiShader->setFloat("alpha", 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    // East (90°)
    float eastHeading = heading - 90.0f;
    if (eastHeading < 0) eastHeading += 360.0f;
    float eastPos = (eastHeading / 360.0f) * width;
    if (eastPos >= 0 && eastPos <= width) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x + eastPos - cardinalWidth/2, y + height*0.2f, 0.0f));
        model = glm::scale(model, glm::vec3(cardinalWidth, cardinalHeight, 1.0f));
        barProjection = projection * model;
        uiShader->setMat4("projection", barProjection);
        uiShader->setVec3("color", glm::vec3(0.9f, 0.9f, 0.9f)); // White for East
        uiShader->setFloat("alpha", 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    // Center indicator (current heading)
    float centerX = x + width / 2.0f;
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(centerX - 1.0f, y, 0.0f));
    model = glm::scale(model, glm::vec3(2.0f, height, 1.0f));
    barProjection = projection * model;
    uiShader->setMat4("projection", barProjection);
    uiShader->setVec3("color", glm::vec3(1.0f, 1.0f, 0.0f)); // Yellow indicator
    uiShader->setFloat("alpha", 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void HUD::updateWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

float HUD::yawToHeading(float yaw) {
    // Convert OpenGL yaw (where -90° is facing positive Z) to compass heading (0° = North)
    float heading = 90.0f - yaw; // Convert to standard compass heading
    
    // Normalize to 0-360 range
    while (heading < 0.0f) heading += 360.0f;
    while (heading >= 360.0f) heading -= 360.0f;
    
    return heading;
}