# Test executables
add_executable(test_camera TestCamera.cpp ../Source/Camera.cpp ../Source/Config.cpp)
add_executable(test_perlin TestPerlin.cpp ../Source/Perlin.cpp)
add_executable(test_biome TestBiome.cpp ../Source/Biome.cpp ../Source/Perlin.cpp)

# Link test libraries
target_link_libraries(test_camera GTest::gtest GTest::gtest_main glm::glm)
target_link_libraries(test_perlin GTest::gtest GTest::gtest_main ${Boost_LIBRARIES})
target_link_libraries(test_biome GTest::gtest GTest::gtest_main ${Boost_LIBRARIES} glm::glm)

# Include directories
target_include_directories(test_camera PRIVATE ../Include ${Boost_INCLUDE_DIRS})
target_include_directories(test_perlin PRIVATE ../Include ${Boost_INCLUDE_DIRS})
target_include_directories(test_biome PRIVATE ../Include ${Boost_INCLUDE_DIRS})

# Add tests
add_test(NAME CameraTest COMMAND test_camera)
add_test(NAME PerlinTest COMMAND test_perlin)
add_test(NAME BiomeTest COMMAND test_biome)