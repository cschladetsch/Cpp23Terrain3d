cmake_minimum_required(VERSION 3.20)
project(Terrain3D)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Boost REQUIRED)
find_package(glm REQUIRED)

# Enable testing
enable_testing()
find_package(GTest REQUIRED)

add_executable(terrain3d 
    Source/Main.cpp
    Source/Camera.cpp
    Source/Config.cpp
    Source/Shader.cpp
    Source/Perlin.cpp
    Source/TerrainChunk.cpp
    Source/DynamicTerrain.cpp
    Source/Biome.cpp
    Source/Water.cpp
    Source/Skybox.cpp
    Source/HUD.cpp
)

target_include_directories(terrain3d PRIVATE 
    ${CMAKE_SOURCE_DIR}/Include
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(terrain3d 
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${Boost_LIBRARIES}
    glm::glm
)

# Copy shaders to build directory
configure_file(${CMAKE_SOURCE_DIR}/Shaders/terrain.vert ${CMAKE_BINARY_DIR}/Shaders/terrain.vert COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/Shaders/terrain.frag ${CMAKE_BINARY_DIR}/Shaders/terrain.frag COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/Shaders/shadow.vert ${CMAKE_BINARY_DIR}/Shaders/shadow.vert COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/Shaders/shadow.frag ${CMAKE_BINARY_DIR}/Shaders/shadow.frag COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/Shaders/water.vert ${CMAKE_BINARY_DIR}/Shaders/water.vert COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/Shaders/water.frag ${CMAKE_BINARY_DIR}/Shaders/water.frag COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/Shaders/skybox.vert ${CMAKE_BINARY_DIR}/Shaders/skybox.vert COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/Shaders/skybox.frag ${CMAKE_BINARY_DIR}/Shaders/skybox.frag COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/Shaders/simple.vert ${CMAKE_BINARY_DIR}/Shaders/simple.vert COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/Shaders/simple.frag ${CMAKE_BINARY_DIR}/Shaders/simple.frag COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/Shaders/ui.vert ${CMAKE_BINARY_DIR}/Shaders/ui.vert COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/Shaders/ui.frag ${CMAKE_BINARY_DIR}/Shaders/ui.frag COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/Shaders/compass.frag ${CMAKE_BINARY_DIR}/Shaders/compass.frag COPYONLY)

# Copy config file to build directory
configure_file(${CMAKE_SOURCE_DIR}/config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)

# Unit tests
add_subdirectory(Test)
