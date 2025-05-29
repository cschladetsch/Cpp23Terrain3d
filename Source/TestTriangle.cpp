#include <GL/glew.h>
#include <iostream>

static GLuint testVAO = 0, testVBO = 0;
static bool initialized = false;

void initTestTriangle() {
    if (initialized) return;
    
    float vertices[] = {
        // positions
        0.0f, 30.0f, -10.0f,
        -20.0f, 10.0f, -10.0f,
        20.0f, 10.0f, -10.0f
    };
    
    glGenVertexArrays(1, &testVAO);
    glGenBuffers(1, &testVBO);
    
    glBindVertexArray(testVAO);
    glBindBuffer(GL_ARRAY_BUFFER, testVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    initialized = true;
    
    std::cout << "Test triangle initialized" << std::endl;
}

void drawTestTriangle() {
    if (!initialized) initTestTriangle();
    
    glBindVertexArray(testVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}