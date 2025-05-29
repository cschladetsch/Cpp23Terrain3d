#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    
    float yaw;
    float pitch;
    float currentSpeed;
    float acceleration;
    float maxSpeed;
    float mouseSensitivity;
    float turnSpeed;
    
    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f, float pitch = 0.0f);
    
    glm::mat4 getViewMatrix() const;
    void accelerate(float deltaTime);
    void decelerate(float deltaTime);
    void turnLeft(float deltaTime);
    void turnRight(float deltaTime);
    void pitchUp(float deltaTime);
    void pitchDown(float deltaTime);
    void update(float deltaTime);
    void constrainToTerrain(float terrainHeight, float minHeightAboveTerrain);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void updateCameraVectors();
    
    float getSpeedKmh() const { return currentSpeed * 3.6f; }
};