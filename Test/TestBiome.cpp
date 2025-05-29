#include <gtest/gtest.h>
#include "Biome.h"

class BiomeTest : public ::testing::Test {
protected:
    void SetUp() override {
        biomeGen = std::make_unique<BiomeGenerator>(12345);
        heightNoise = std::make_unique<PerlinNoise>(42);
    }
    
    std::unique_ptr<BiomeGenerator> biomeGen;
    std::unique_ptr<PerlinNoise> heightNoise;
};

TEST_F(BiomeTest, BiomeTypeConsistency) {
    // Same location should always return same biome
    BiomeType b1 = biomeGen->getBiome(100.0f, 200.0f);
    BiomeType b2 = biomeGen->getBiome(100.0f, 200.0f);
    
    EXPECT_EQ(b1, b2);
}

TEST_F(BiomeTest, BiomeParameters) {
    // Each biome should have valid parameters
    BiomeParams desert = biomeGen->getBiomeParams(BiomeType::DESERT);
    EXPECT_GT(desert.heightScale, 0.0f);
    EXPECT_GT(desert.roughness, 0.0f);
    EXPECT_LE(desert.roughness, 1.0f);
    
    BiomeParams mountains = biomeGen->getBiomeParams(BiomeType::MOUNTAINS);
    EXPECT_GT(mountains.heightScale, desert.heightScale); // Mountains should be taller
}

TEST_F(BiomeTest, HeightGeneration) {
    // Height should be within reasonable bounds
    float height = biomeGen->generateHeight(0.0f, 0.0f, *heightNoise);
    
    EXPECT_GT(height, -100.0f);
    EXPECT_LT(height, 200.0f);
}

TEST_F(BiomeTest, ColorGeneration) {
    // Colors should be valid (0-1 range)
    glm::vec3 color = biomeGen->getColor(0.0f, 0.0f, 10.0f, *heightNoise);
    
    EXPECT_GE(color.r, 0.0f);
    EXPECT_LE(color.r, 1.0f);
    EXPECT_GE(color.g, 0.0f);
    EXPECT_LE(color.g, 1.0f);
    EXPECT_GE(color.b, 0.0f);
    EXPECT_LE(color.b, 1.0f);
}

TEST_F(BiomeTest, BiomeBlending) {
    BiomeType primary, secondary;
    float blend = biomeGen->getBiomeBlend(0.0f, 0.0f, primary, secondary);
    
    // Blend should be between 0 and 1
    EXPECT_GE(blend, 0.0f);
    EXPECT_LE(blend, 1.0f);
}

TEST_F(BiomeTest, DifferentBiomes) {
    // Test that we can get different biomes at different locations
    std::set<BiomeType> foundBiomes;
    
    for (int x = -1000; x <= 1000; x += 200) {
        for (int z = -1000; z <= 1000; z += 200) {
            BiomeType biome = biomeGen->getBiome(x, z);
            foundBiomes.insert(biome);
        }
    }
    
    // Should find at least 2 different biomes in this range
    EXPECT_GE(foundBiomes.size(), 2);
}