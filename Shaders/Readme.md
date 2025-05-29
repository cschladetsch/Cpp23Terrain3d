# Shaders Directory

This directory contains all GLSL shader files for the 3D Terrain Flyover project.

## Shader Overview

All shaders use **OpenGL 3.3 Core Profile** with GLSL version `#version 330 core`.

## Terrain Rendering

### `terrain.vert` & `terrain.frag`
**Purpose**: Main terrain chunk rendering with biome colors, shadows, and fog
- **Vertex Shader**: Transforms terrain vertices, calculates shadow map coordinates
- **Fragment Shader**: Applies biome colors, shadow mapping, and exponential distance fog
- **Features**:
  - Height-based terrain coloring
  - Real-time shadow mapping with PCF soft shadows
  - Exponential fog blending to skybox colors
  - Biome color mixing for realistic terrain appearance

## Water Rendering

### `water.vert` & `water.frag`
**Purpose**: Realistic water surface with reflections and refractions
- **Vertex Shader**: Water surface vertex positioning with wave displacement
- **Fragment Shader**: Combines reflection/refraction textures with Fresnel effects
- **Features**:
  - Reflection and refraction texture sampling
  - Wave-based normal perturbation
  - Fresnel effect for realistic water appearance
  - Specular highlights and depth-based transparency
  - Distance fog integration

## Melbourne Night Sky

### `skybox.vert` & `skybox.frag`
**Purpose**: Procedural Melbourne night sky with realistic stars
- **Vertex Shader**: Skybox cube rendering without translation
- **Fragment Shader**: Generates atmospheric night sky with stars
- **Features**:
  - 25+ major stars visible from Melbourne, Australia
  - Southern Cross constellation with accurate positioning
  - Magnitude-based star sizing (brighter = larger)
  - Realistic twinkling/scintillation effects
  - Atmospheric scintillation (stars near horizon twinkle more)
  - Diffraction spikes for bright stars
  - Procedural background star field using noise

## Shadow Mapping

### `shadow.vert` & `shadow.frag`
**Purpose**: Depth buffer generation for shadow mapping
- **Vertex Shader**: Transforms vertices to light space for depth testing
- **Fragment Shader**: Simple depth output for shadow map creation
- **Features**:
  - Orthographic light projection for sun shadows
  - Optimized for terrain shadow casting
  - Used by terrain shader for shadow lookup

## User Interface

### `ui.vert` & `ui.frag`
**Purpose**: Basic 2D UI element rendering
- **Vertex Shader**: 2D orthographic projection for UI elements
- **Fragment Shader**: Solid color rendering with alpha blending
- **Features**:
  - Orthographic projection for pixel-perfect UI
  - Alpha blending support for transparent panels
  - Simple color and alpha uniforms

### `compass.frag`
**Purpose**: Specialized compass rendering with aviation-style design
- **Fragment Shader**: Generates circular compass with heading needle
- **Features**:
  - Circular compass background with tick marks
  - Cardinal direction markers (N/E/S/W)
  - Yellow heading needle showing aircraft direction
  - Red North marker for orientation reference
  - Smooth circle generation using distance fields

## Legacy/Utility

### `simple.vert` & `simple.frag`
**Purpose**: Basic geometry rendering for testing and debugging
- **Vertex Shader**: Simple vertex transformation
- **Fragment Shader**: Solid color output
- **Usage**: Development and debugging geometry

## Shader Features

### Fog System
Multiple shaders implement consistent fog effects:
```glsl
float distance = length(FragPos - viewPos);
float fogFactor = 1.0 - exp(-fogDensity * max(0.0, distance - fogStart));
fogFactor = clamp(fogFactor, 0.0, 1.0);
result = mix(result, fogColor, fogFactor);
```

### Shadow Mapping
Terrain shader uses PCF (Percentage Closer Filtering) for soft shadows:
```glsl
float shadow = 0.0;
vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
for(int x = -1; x <= 1; ++x) {
    for(int y = -1; y <= 1; ++y) {
        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
}
shadow /= 9.0;
```

### Star Rendering
Skybox shader implements realistic star effects:
```glsl
float twinkle = 0.7 + 0.3 * sin(time * twinkleSpeed + float(i) * 12.34);
float starSize = baseStarSize * twinkle;
float intensity = pow(1.0 - distance / starSize, 2.0) * visibility;
```

## Performance Considerations

- **Minimal Branching**: Shaders avoid complex conditional logic
- **Efficient Sampling**: Texture lookups optimized for cache coherency  
- **LOD Aware**: Shaders handle different detail levels gracefully
- **Fog Optimization**: Exponential fog more efficient than linear
- **Star Culling**: Stars fade based on sky brightness naturally