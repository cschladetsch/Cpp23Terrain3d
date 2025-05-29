#include "Camera.h"
#include "Config.h"
#include <algorithm>

Camera::Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch)
    : position(pos), worldUp(up), yaw(yaw), pitch(pitch),
      currentSpeed(0.0f) {
    Config& config = Config::getInstance();
    acceleration = config.camera.acceleration;
    maxSpeed = config.camera.maxSpeed;
    mouseSensitivity = config.camera.mouseSensitivity;
    turnSpeed = config.camera.turnSpeed;
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void Camera::accelerate(float deltaTime) {
    currentSpeed += acceleration * deltaTime;
    currentSpeed = std::min(currentSpeed, maxSpeed);
}

void Camera::decelerate(float deltaTime) {
    Config& config = Config::getInstance();
    currentSpeed -= acceleration * deltaTime;
    currentSpeed = std::max(currentSpeed, -maxSpeed * config.camera.reverseSpeedFactor);
}

void Camera::turnLeft(float deltaTime) {
    float turnAmount = turnSpeed * deltaTime;
    // Reduce turn rate at higher speeds for more realistic flight
    turnAmount *= (1.0f - std::abs(currentSpeed) / maxSpeed * 0.5f);
    yaw -= turnAmount;
    updateCameraVectors();
}

void Camera::turnRight(float deltaTime) {
    float turnAmount = turnSpeed * deltaTime;
    // Reduce turn rate at higher speeds for more realistic flight
    turnAmount *= (1.0f - std::abs(currentSpeed) / maxSpeed * 0.5f);
    yaw += turnAmount;
    updateCameraVectors();
}

void Camera::pitchUp(float deltaTime) {
    float pitchAmount = turnSpeed * deltaTime;
    // Reduce pitch rate at higher speeds for more realistic flight
    pitchAmount *= (1.0f - std::abs(currentSpeed) / maxSpeed * 0.5f);
    pitch += pitchAmount;
    if (pitch > 89.0f) pitch = 89.0f;
    updateCameraVectors();
}

void Camera::pitchDown(float deltaTime) {
    float pitchAmount = turnSpeed * deltaTime;
    // Reduce pitch rate at higher speeds for more realistic flight
    pitchAmount *= (1.0f - std::abs(currentSpeed) / maxSpeed * 0.5f);
    pitch -= pitchAmount;
    if (pitch < -89.0f) pitch = -89.0f;
    updateCameraVectors();
}

void Camera::update(float deltaTime) {
    Config& config = Config::getInstance();
    // Apply slight deceleration when no input (air resistance)
    if (std::abs(currentSpeed) > 0.1f) {
        float deceleration = config.camera.airResistance * deltaTime;
        if (currentSpeed > 0) {
            currentSpeed = std::max(0.0f, currentSpeed - deceleration);
        } else {
            currentSpeed = std::min(0.0f, currentSpeed + deceleration);
        }
    }
    
    // Move camera based on current speed in full 3D
    position += front * currentSpeed * deltaTime;
}

void Camera::constrainToTerrain(float terrainHeight, float minHeightAboveTerrain) {
    float minHeight = terrainHeight + minHeightAboveTerrain;
    if (position.y < minHeight) {
        position.y = minHeight;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    
    yaw += xoffset;
    pitch += yoffset;
    
    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
    
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}