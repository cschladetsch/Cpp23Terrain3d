#include <gtest/gtest.h>
#include "Camera.h"
#include "Config.h"
#include <glm/gtc/epsilon.hpp>

class CameraTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create minimal config for testing
        Config& config = Config::getInstance();
        config.camera.acceleration = 20.0f;
        config.camera.maxSpeed = 100.0f;
        config.camera.reverseSpeedFactor = 0.5f;
        config.camera.mouseSensitivity = 0.1f;
        config.camera.turnSpeed = 50.0f;
        config.camera.airResistance = 5.0f;
        
        camera = std::make_unique<Camera>(glm::vec3(0.0f, 10.0f, 0.0f));
    }
    
    std::unique_ptr<Camera> camera;
};

TEST_F(CameraTest, InitialState) {
    EXPECT_FLOAT_EQ(camera->currentSpeed, 0.0f);
    EXPECT_FLOAT_EQ(camera->position.y, 10.0f);
    EXPECT_FLOAT_EQ(camera->yaw, -90.0f);
    EXPECT_FLOAT_EQ(camera->pitch, 0.0f);
}

TEST_F(CameraTest, Acceleration) {
    float deltaTime = 0.1f;
    camera->accelerate(deltaTime);
    
    EXPECT_GT(camera->currentSpeed, 0.0f);
    EXPECT_LE(camera->currentSpeed, camera->maxSpeed);
}

TEST_F(CameraTest, Deceleration) {
    float deltaTime = 0.1f;
    
    // First accelerate
    for (int i = 0; i < 10; ++i) {
        camera->accelerate(deltaTime);
    }
    float speedBefore = camera->currentSpeed;
    
    // Then decelerate
    camera->decelerate(deltaTime);
    EXPECT_LT(camera->currentSpeed, speedBefore);
}

TEST_F(CameraTest, ReverseSpeed) {
    float deltaTime = 0.1f;
    
    // Decelerate from standstill should go negative (reverse)
    for (int i = 0; i < 20; ++i) {
        camera->decelerate(deltaTime);
    }
    
    EXPECT_LT(camera->currentSpeed, 0.0f);
    EXPECT_GE(camera->currentSpeed, -camera->maxSpeed * 0.5f);
}

TEST_F(CameraTest, Turning) {
    float deltaTime = 0.1f;
    float initialYaw = camera->yaw;
    
    camera->turnLeft(deltaTime);
    EXPECT_LT(camera->yaw, initialYaw);
    
    camera->turnRight(deltaTime);
    camera->turnRight(deltaTime);
    EXPECT_GT(camera->yaw, initialYaw);
}

TEST_F(CameraTest, PositionUpdate) {
    float deltaTime = 0.1f;
    glm::vec3 initialPos = camera->position;
    
    // Accelerate and update
    camera->accelerate(deltaTime);
    camera->update(deltaTime);
    
    // Should have moved forward
    EXPECT_NE(camera->position, initialPos);
}

TEST_F(CameraTest, SpeedConversion) {
    camera->currentSpeed = 10.0f; // 10 m/s
    EXPECT_FLOAT_EQ(camera->getSpeedKmh(), 36.0f); // Should be 36 km/h
}

TEST_F(CameraTest, AirResistance) {
    float deltaTime = 0.1f;
    
    // Accelerate to some speed
    for (int i = 0; i < 5; ++i) {
        camera->accelerate(deltaTime);
    }
    float speedBefore = camera->currentSpeed;
    
    // Update without input - should decelerate due to air resistance
    camera->update(deltaTime);
    EXPECT_LT(camera->currentSpeed, speedBefore);
}