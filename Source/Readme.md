# Source Directory

This directory contains all implementation files (.cpp) for the 3D Terrain Flyover project.

## Core Implementation Files

### Application Entry Point
- **`Main.cpp`** - Main application class with SDL2 setup, render loop, and input handling

### Graphics & Rendering
- **`Camera.cpp`** - Flight camera implementation with 3D movement, pitch/yaw controls, and terrain collision
- **`Shader.cpp`** - OpenGL shader compilation, linking, and uniform management
- **`Skybox.cpp`** - Procedural Melbourne night sky with 25+ major stars, twinkling effects, and Southern Cross
- **`HUD.cpp`** - Compass rendering, heading display, and flight instrumentation UI

### Terrain System
- **`DynamicTerrain.cpp`** - Infinite terrain management with 32-chunk view distance and 4-level LOD system
- **`TerrainChunk.cpp`** - Individual chunk mesh generation, edge stitching, and visibility culling
- **`Perlin.cpp`** - Multi-octave Perlin noise with continental, regional, and local detail layers
- **`Biome.cpp`** - Biome generation with smooth transitions and height-based coloring

### Water & Effects
- **`Water.cpp`** - Water surface rendering with reflections, refractions, wave simulation, and fog effects

### Utilities & Configuration
- **`Config.cpp`** - JSON configuration loading with validation and default values

### Legacy/Deprecated
- **`Terrain.cpp`** - Original terrain renderer (replaced by DynamicTerrain)
- **`TestTriangle.cpp`** - Basic OpenGL test geometry (development artifact)

## Key Features Implemented

### Flight Controls
- **3D Movement**: Full pitch, yaw, and velocity-based flight
- **Terrain Following**: Automatic height constraint to stay above ground
- **Speed Management**: Acceleration/deceleration with air resistance
- **Heading Indicators**: Real-time compass and numerical heading display

### Infinite Terrain
- **Dynamic Loading**: Chunks load/unload based on camera position
- **LOD System**: 4 levels of detail for performance optimization
- **Edge Stitching**: Seamless connections between different LOD chunks
- **Fog Effects**: Exponential distance fog blending to skybox

### Melbourne Night Sky
- **Astronomical Accuracy**: Real star positions and magnitudes for Melbourne (-37.8° S)
- **Twinkling Effects**: Realistic atmospheric scintillation
- **Southern Cross**: Prominent constellation with accurate positioning
- **Magnitude-based Sizing**: Brighter stars appear larger

### Performance Optimizations
- **Frustum Culling**: Only render visible chunks (currently disabled for stability)
- **Distance Culling**: Automatic chunk unloading beyond view distance
- **Chunk Pooling**: Memory reuse to prevent allocation overhead
- **Efficient Rendering**: Batched draw calls and optimized shaders

## Build Dependencies

All source files require:
- **C++23 Compiler**: GCC 12+ or Clang 15+
- **OpenGL 3.3+**: Core profile with shader support
- **SDL2**: Window management and input
- **GLEW**: OpenGL extension loading
- **GLM**: Vector/matrix mathematics
- **Boost**: General utilities (minimal usage)

## Performance Characteristics

- **Target**: 60+ FPS on RTX 2070
- **Memory**: ~200MB for 32-chunk view distance
- **CPU**: Single-threaded with potential for multi-threading
- **GPU**: Optimized for modern discrete graphics cards