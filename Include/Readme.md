# Include Directory

This directory contains all header files for the 3D Terrain Flyover project.

## Core System Headers

### Graphics & Rendering
- **`Camera.h`** - First-person flight camera with pitch/yaw controls and velocity-based movement
- **`Shader.h`** - OpenGL shader wrapper with uniform management and compilation utilities
- **`Skybox.h`** - Procedural Melbourne night sky with realistic star rendering and atmospheric effects
- **`HUD.h`** - Heads-Up Display system with compass, heading indicators, and flight instrumentation

### Terrain System
- **`DynamicTerrain.h`** - Infinite terrain manager with chunk loading/unloading and LOD system
- **`TerrainChunk.h`** - Individual terrain chunk with mesh generation and frustum culling
- **`Perlin.h`** - Multi-octave Perlin noise generator for realistic terrain features
- **`Biome.h`** - Biome system with desert, forest, mountain, and tundra generation

### Water & Effects
- **`Water.h`** - Realistic water rendering with reflections, refractions, and wave simulation
- **`Terrain.h`** - Legacy terrain renderer (deprecated in favor of DynamicTerrain)

### Utilities & Configuration
- **`Config.h`** - JSON-based configuration system for all game parameters
- **`Json.hpp`** - Third-party JSON library (nlohmann/json) for configuration parsing
- **`StbImage.h`** - STB image library for texture loading

## Architecture Overview

The project follows a component-based architecture:

```
Camera ←→ DynamicTerrain ←→ TerrainChunk
   ↓            ↓               ↓
 HUD         Biome          Perlin
   ↓            ↓               ↓
Shader       Water          Config
```

## Key Design Patterns

- **RAII**: All OpenGL resources managed with RAII destructors
- **Smart Pointers**: Extensive use of `std::unique_ptr` for memory management
- **Component System**: Modular design allows easy addition of new features
- **Configuration-Driven**: All parameters externalized to JSON configuration

## Dependencies

- **OpenGL 3.3+**: Core graphics rendering
- **GLM**: Mathematics library for vectors, matrices, and transformations
- **SDL2**: Window management and input handling
- **GLEW**: OpenGL extension loading
- **nlohmann/json**: JSON configuration parsing

## Thread Safety

- Most classes are **not thread-safe** by design for performance
- Terrain generation could be multi-threaded in future versions
- All OpenGL calls must be made from the main thread