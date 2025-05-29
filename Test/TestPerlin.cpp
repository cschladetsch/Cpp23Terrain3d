#include <gtest/gtest.h>
#include "Perlin.h"

class PerlinTest : public ::testing::Test {
protected:
    void SetUp() override {
        perlin = std::make_unique<PerlinNoise>(42);
    }
    
    std::unique_ptr<PerlinNoise> perlin;
};

TEST_F(PerlinTest, NoiseRange) {
    // Perlin noise should be in range [-1, 1]
    for (int i = 0; i < 100; ++i) {
        float x = i * 0.1f;
        float y = i * 0.2f;
        float z = i * 0.3f;
        
        double value = perlin->noise(x, y, z);
        EXPECT_GE(value, -1.0);
        EXPECT_LE(value, 1.0);
    }
}

TEST_F(PerlinTest, Smoothness) {
    // Adjacent values should be similar (continuity)
    double v1 = perlin->noise(1.0, 1.0, 1.0);
    double v2 = perlin->noise(1.01, 1.0, 1.0);
    
    EXPECT_LT(std::abs(v1 - v2), 0.1);
}

TEST_F(PerlinTest, Deterministic) {
    // Same input should give same output
    double v1 = perlin->noise(5.5, 3.2, 1.7);
    double v2 = perlin->noise(5.5, 3.2, 1.7);
    
    EXPECT_DOUBLE_EQ(v1, v2);
}

TEST_F(PerlinTest, DifferentSeeds) {
    PerlinNoise perlin2(123);
    
    double v1 = perlin->noise(1.5, 2.3, 3.7);
    double v2 = perlin2.noise(1.5, 2.3, 3.7);
    
    // Different seeds should generally give different values
    EXPECT_NE(v1, v2);
}

TEST_F(PerlinTest, OctaveNoise) {
    // Octave noise should also be in a reasonable range
    double value = perlin->octaveNoise(1.0, 2.0, 3.0, 4, 0.5);
    
    // With persistence 0.5 and 4 octaves, max theoretical range is about [-1.875, 1.875]
    EXPECT_GE(value, -2.0);
    EXPECT_LE(value, 2.0);
}

TEST_F(PerlinTest, ZeroOctaves) {
    // Zero octaves should return 0
    double value = perlin->octaveNoise(1.0, 2.0, 3.0, 0, 0.5);
    EXPECT_DOUBLE_EQ(value, 0.0);
}