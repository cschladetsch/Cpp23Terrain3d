# Test Directory

This directory contains unit tests for the 3D Terrain Flyover project using Google Test framework.

## Test Files

### Core Component Tests

#### `TestPerlin.cpp`
**Purpose**: Tests the Perlin noise generation system
- **Functions Tested**:
  - `octaveNoise()` - Multi-octave noise generation
  - `noise()` - Basic Perlin noise
  - Noise consistency and reproducibility
- **Test Cases**:
  - Deterministic output for same seed/coordinates
  - Proper range validation (-1.0 to 1.0)
  - Octave parameter effects
  - Performance benchmarks

#### `TestCamera.cpp`
**Purpose**: Tests camera movement and control systems
- **Functions Tested**:
  - `accelerate()` / `decelerate()` - Speed control
  - `turnLeft()` / `turnRight()` - Yaw rotation
  - `pitchUp()` / `pitchDown()` - Pitch control
  - `getViewMatrix()` - Matrix generation
- **Test Cases**:
  - Movement boundaries and limits
  - Speed calculations (km/h conversion)
  - Camera matrix correctness
  - Input response timing

#### `TestBiome.cpp`
**Purpose**: Tests biome generation and color systems
- **Functions Tested**:
  - `generateHeight()` - Biome-specific height generation
  - `getColor()` - Height-based color interpolation
  - Biome transition smoothness
- **Test Cases**:
  - Biome boundary detection
  - Color gradient validation
  - Height scaling correctness
  - Performance under load

## Test Configuration

### `test_config.json`
**Purpose**: Test-specific configuration overrides
- Reduced terrain parameters for faster test execution
- Disabled graphics features not needed for unit tests
- Simplified biome configurations for predictable testing

### `CMakeLists.txt`
**Purpose**: Test build configuration
- Links Google Test framework
- Creates individual test executables
- Configures CTest integration

## Running Tests

### All Tests
```bash
cd build
ctest
```

### Individual Tests
```bash
cd build
./test_perlin
./test_camera
./test_biome
```

### Verbose Output
```bash
cd build
ctest --verbose
```

## Test Coverage

### Currently Tested
- ✅ **Perlin Noise Generation**: Deterministic output, range validation
- ✅ **Camera Controls**: Movement, rotation, matrix generation
- ✅ **Biome System**: Height generation, color interpolation

### Not Yet Tested
- ❌ **Terrain Chunks**: Mesh generation, LOD transitions
- ❌ **Shader System**: Compilation, uniform setting
- ❌ **Water Simulation**: Wave generation, reflections
- ❌ **HUD System**: Compass rendering, heading calculations
- ❌ **Configuration**: JSON parsing, validation

## Test Philosophy

### Unit Testing Approach
- **Isolated Testing**: Each component tested independently
- **Deterministic Results**: All tests produce consistent, repeatable results
- **Performance Awareness**: Tests include basic performance validation
- **Edge Case Coverage**: Boundary conditions and error states tested

### Graphics Testing Challenges
- **No OpenGL Context**: Unit tests run without graphics initialization
- **Mock Objects**: Graphics-dependent features tested with mocks where possible
- **Integration Tests**: Full rendering pipeline tested manually

## Adding New Tests

### Test File Template
```cpp
#include <gtest/gtest.h>
#include "YourClass.h"

class YourClassTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }
    
    void TearDown() override {
        // Test cleanup
    }
};

TEST_F(YourClassTest, TestBasicFunctionality) {
    // Test implementation
    EXPECT_EQ(expected, actual);
}
```

### CMakeLists.txt Integration
```cmake
add_executable(test_yourclass TestYourClass.cpp ../Source/YourClass.cpp)
target_link_libraries(test_yourclass ${GTEST_LIBRARIES} pthread)
add_test(NAME test_yourclass COMMAND test_yourclass)
```

## Performance Benchmarks

Tests include basic performance validation:
- **Perlin Noise**: >1M samples/second on development hardware
- **Camera Updates**: <1ms per update for smooth 60Hz operation
- **Biome Generation**: <10ms for full chunk biome assignment

## Continuous Integration

Tests are designed to run in CI environments:
- **No Graphics Dependencies**: All tests run in headless mode
- **Fast Execution**: Complete test suite runs in <30 seconds
- **Deterministic**: No random failures or flaky tests
- **Cross-Platform**: Tests run on Windows, Linux, and macOS