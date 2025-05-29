# 3D Terrain Flyover

A high-performance C++23 terrain renderer featuring dynamic world generation, realistic biomes, water simulation, and smooth flight controls. Optimized for 60+ FPS on RTX 2070.

## Features

### Terrain Generation
- **Dynamic chunk-based loading** - Infinite terrain that loads/unloads based on camera position
- **4 distinct biomes** with smooth transitions:
  - 🏜️ **Desert** - Rolling sand dunes with orange-yellow gradient
  - 🌲 **Forest** - Lush green hills with varied terrain
  - 🏔️ **Mountains** - Towering peaks with snow caps
  - ❄️ **Tundra** - Icy plains with permafrost coloring
- **Multi-scale Perlin noise** for realistic terrain features
- **Level of Detail (LOD)** system for distant chunks

### Rendering Features
- **Real-time shadow mapping** with soft shadows
- **Water rendering** with reflections and refractions
- **Procedural skybox** with gradient colors
- **Biome-specific coloring** with height-based variations

### Flight Controls
- **Velocity-based movement** with acceleration/deceleration
- **Speed indicator** in window title (km/h)
- **Automatic terrain following** to prevent flying through ground
- **Mouse look** for precise camera control
- **Smooth turning** with speed-dependent turn rates

## Building

### Prerequisites
```bash
# Ubuntu/Debian/WSL
sudo apt-get update
sudo apt-get install -y \
    build-essential cmake \
    libsdl2-dev libglew-dev \
    libboost-all-dev libglm-dev \
    libgtest-dev
```

### Build Steps
```bash
./build.sh
```

Or manually:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Controls

| Key/Input | Action |
|-----------|--------|
| **W** | Accelerate forward |
| **S** | Decelerate / Reverse |
| **A** | Turn left |
| **D** | Turn right |
| **Mouse** | Look around (click to capture) |
| **ESC** | Exit |

## Performance

Optimized for smooth 60Hz gameplay on RTX 2070:
- Reduced shadow resolution for performance
- Half-resolution water reflections
- Efficient chunk pooling system
- LOD system reduces polygon count for distant terrain

## Testing

Run unit tests:
```bash
cd build
ctest
```

## Technical Details

- **Language**: C++23
- **Graphics API**: OpenGL 3.3 Core
- **Platform**: Cross-platform (tested on Windows 11 WSL)
- **Architecture**: Component-based with dynamic terrain chunks

## License

MIT License - see [LICENSE](LICENSE) file for details.