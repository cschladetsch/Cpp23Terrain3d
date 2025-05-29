#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <chrono>
#include <algorithm>
#include "Camera.h"
#include "DynamicTerrain.h"
#include "Water.h"
#include "Skybox.h"
#include "Shader.h"
#include "Config.h"
#include "HUD.h"

class Application {
private:
    SDL_Window* window = nullptr;
    SDL_GLContext glContext;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<DynamicTerrain> terrain;
    std::unique_ptr<Water> water;
    std::unique_ptr<Skybox> skybox;
    std::unique_ptr<Shader> terrainShader;
    std::unique_ptr<Shader> shadowShader;
    std::unique_ptr<HUD> hud;
    
    bool running = true;
    bool keys[SDL_NUM_SCANCODES] = {false};
    float lastDisplayedSpeed = 0.0f;
    
    GLuint shadowMapFBO;
    GLuint shadowMap;
    Config& config = Config::getInstance();
    
    // Debug quad
    GLuint debugVAO, debugVBO;
    bool debugQuadInit = false;
    
public:
    bool init() {
        // Load configuration
        try {
            config.loadFromFile("config.json");
        } catch (const std::exception& e) {
            std::cerr << "Failed to load config: " << e.what() << std::endl;
            return false;
        }
        
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        
        window = SDL_CreateWindow(config.window.title.c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            config.window.width, config.window.height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | (config.window.fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
            
        if (!window) {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        glContext = SDL_GL_CreateContext(window);
        if (!glContext) {
            std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        SDL_GL_SetSwapInterval(config.window.vsync ? 1 : 0);
        
        if (glewInit() != GLEW_OK) {
            std::cerr << "GLEW initialization failed" << std::endl;
            return false;
        }
        
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE); // Temporarily disable culling to debug
        
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
        
        camera = std::make_unique<Camera>(config.camera.initialPosition, glm::vec3(0.0f, 1.0f, 0.0f),
                                         config.camera.initialYaw, config.camera.initialPitch);
        terrain = std::make_unique<DynamicTerrain>();
        std::cout << "Terrain system initialized" << std::endl;
        
        if (config.rendering.enableWater) {
            water = std::make_unique<Water>(config.window.width, config.window.height);
        }
        
        if (config.rendering.enableSkybox) {
            skybox = std::make_unique<Skybox>();
        }
        
        terrainShader = std::make_unique<Shader>("Shaders/terrain.vert", "Shaders/terrain.frag");
        shadowShader = std::make_unique<Shader>("Shaders/shadow.vert", "Shaders/shadow.frag");
        
        hud = std::make_unique<HUD>(config.window.width, config.window.height);
        
        setupShadowMap();
        
        
        std::cout << "\nControls:" << std::endl;
        std::cout << "  W/S - Accelerate/Decelerate" << std::endl;
        std::cout << "  A/D - Turn left/right" << std::endl;
        std::cout << "  Z/X - Pitch up/down" << std::endl;
        std::cout << "  ESC - Exit" << std::endl;
        
        return true;
    }
    
    void setupShadowMap() {
        if (!config.rendering.enableShadows) return;
        
        glGenFramebuffers(1, &shadowMapFBO);
        
        glGenTextures(1, &shadowMap);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
            config.rendering.shadowMapResolution, config.rendering.shadowMapResolution, 
            0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    keys[event.key.keysym.scancode] = true;
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                    break;
                case SDL_KEYUP:
                    keys[event.key.keysym.scancode] = false;
                    break;
            }
        }
    }
    
    void update(float deltaTime) {
        // W/S for speed control
        if (keys[SDL_SCANCODE_W]) {
            camera->accelerate(deltaTime);
        }
        if (keys[SDL_SCANCODE_S]) {
            camera->decelerate(deltaTime);
        }
        
        // A/D for turning
        if (keys[SDL_SCANCODE_A]) {
            camera->turnLeft(deltaTime);
        }
        if (keys[SDL_SCANCODE_D]) {
            camera->turnRight(deltaTime);
        }
        
        // Z/X for pitch
        if (keys[SDL_SCANCODE_Z]) {
            camera->pitchUp(deltaTime);
        }
        if (keys[SDL_SCANCODE_X]) {
            camera->pitchDown(deltaTime);
        }
        
        
        // Calculate proposed movement
        glm::vec3 movement = camera->front * camera->currentSpeed * deltaTime;
        glm::vec3 newPosition = camera->position + movement;
        
        // Check terrain height at new XZ position
        float terrainHeight = terrain->getHeightAt(newPosition.x, newPosition.z);
        float minHeight = terrainHeight + config.camera.minHeightAboveTerrain;
        
        // Ensure new position never goes below minimum height
        newPosition.y = std::max(newPosition.y, minHeight);
        
        // Update camera position (this replaces camera->update)
        camera->position = newPosition;
        
        // Apply air resistance
        if (std::abs(camera->currentSpeed) > 0.1f) {
            float deceleration = config.camera.airResistance * deltaTime;
            if (camera->currentSpeed > 0) {
                camera->currentSpeed = std::max(0.0f, camera->currentSpeed - deceleration);
            } else {
                camera->currentSpeed = std::min(0.0f, camera->currentSpeed + deceleration);
            }
        }
        
        float currentHeight = camera->position.y - terrainHeight;
        
        // Angle camera upward as it approaches the terrain
        float approachThreshold = config.camera.minHeightAboveTerrain * 2.0f; // Start affecting pitch at 2x min height
        if (currentHeight < approachThreshold) {
            float approachFactor = 1.0f - (currentHeight / approachThreshold); // 0 to 1 as we get closer
            float targetPitch = approachFactor * 30.0f; // Max 30 degrees upward
            
            // Gradually adjust pitch upward
            if (camera->pitch < targetPitch) {
                float pitchAdjustment = 60.0f * deltaTime; // 60 degrees per second adjustment rate
                camera->pitch = std::min(camera->pitch + pitchAdjustment, targetPitch);
                camera->updateCameraVectors();
            }
        }
        
        // Update terrain based on camera position
        glm::mat4 projection = glm::perspective(glm::radians(config.rendering.fieldOfView),
            (float)config.window.width / (float)config.window.height, 
            config.rendering.nearPlane, config.rendering.farPlane);
        glm::mat4 view = camera->getViewMatrix();
        terrain->update(camera->position, projection * view);
        
        
        // Update water simulation
        if (water) {
            water->update(deltaTime);
        }
    }
    
    void render() {
        glm::vec3 lightPos = camera->position + config.lighting.sunOffset;
        glm::vec3 lightTarget = camera->position;
        
        glm::mat4 lightProjection = glm::ortho(-config.lighting.shadowOrthoSize, config.lighting.shadowOrthoSize, 
                                               -config.lighting.shadowOrthoSize, config.lighting.shadowOrthoSize, 
                                               config.lighting.shadowNearPlane, config.lighting.shadowFarPlane);
        glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        
        glm::mat4 projection = glm::perspective(glm::radians(config.rendering.fieldOfView),
            (float)config.window.width / (float)config.window.height, 
            config.rendering.nearPlane, config.rendering.farPlane);
        glm::mat4 view = camera->getViewMatrix();
        
        // Shadow pass
        if (config.rendering.enableShadows) {
            glViewport(0, 0, config.rendering.shadowMapResolution, config.rendering.shadowMapResolution);
            glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            shadowShader->use();
            shadowShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
            terrain->render(*shadowShader, *shadowShader, lightSpaceMatrix);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        
        // Reflection pass
        if (water) {
            water->beginReflectionPass();
            float distance = 2 * (camera->position.y - water->getWaterLevel());
            camera->position.y -= distance;
            camera->pitch = -camera->pitch;
            camera->updateCameraVectors();
            
            if (skybox) {
                float currentTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                glm::vec3 sunDir = glm::normalize(lightPos - camera->position);
                skybox->render(camera->getViewMatrix(), projection, currentTime, sunDir);
            }
            renderTerrain(projection, camera->getViewMatrix(), lightSpaceMatrix, lightPos, true);
            
            camera->position.y += distance;
            camera->pitch = -camera->pitch;
            camera->updateCameraVectors();
            water->endReflectionPass();
            
            // Refraction pass
            water->beginRefractionPass();
            if (skybox) {
                float currentTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                glm::vec3 sunDir = glm::normalize(lightPos - camera->position);
                skybox->render(view, projection, currentTime, sunDir);
            }
            renderTerrain(projection, view, lightSpaceMatrix, lightPos, false);
            water->endRefractionPass();
        }
        
        // Main render pass
        glViewport(0, 0, config.window.width, config.window.height);
        glClearColor(0.2f, 0.0f, 0.3f, 1.0f); // Purple background to confirm rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Debug: Make sure culling is disabled
        glDisable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        // Render skybox first
        if (skybox) {
            float currentTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            glm::vec3 sunDir = glm::normalize(lightPos - camera->position);
            skybox->render(view, projection, currentTime, sunDir);
        }
        
        // Render terrain
        renderTerrain(projection, view, lightSpaceMatrix, lightPos, false);
        
        
        
        // Render water
        if (water) {
            water->render(view, projection, camera->position, lightPos, shadowMap,
                         0.00008f, 500.0f, glm::vec3(0.02f, 0.02f, 0.08f));
        }
        
        // Render HUD last (on top of everything)
        if (hud) {
            float heading = HUD::yawToHeading(camera->yaw);
            hud->render(heading, camera->pitch, camera->getSpeedKmh());
        }
        
        SDL_GL_SwapWindow(window);
        
        // Update window title with speed (only when speed changes significantly)
        float currentSpeed = camera->getSpeedKmh();
        if (std::abs(currentSpeed - lastDisplayedSpeed) > 0.5f) {
            updateWindowTitle();
            lastDisplayedSpeed = currentSpeed;
        }
    }
    
    void updateWindowTitle() {
        char title[256];
        float heading = HUD::yawToHeading(camera->yaw);
        snprintf(title, sizeof(title), "3D Terrain Flyover - Heading: %.0f° - Speed: %.1f km/h", heading, camera->getSpeedKmh());
        SDL_SetWindowTitle(window, title);
    }
    
    void renderTerrain(const glm::mat4& projection, const glm::mat4& view,
                       const glm::mat4& lightSpaceMatrix, const glm::vec3& lightPos,
                       bool clipPlane) {
        terrainShader->use();
        terrainShader->setMat4("projection", projection);
        terrainShader->setMat4("view", view);
        terrainShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        terrainShader->setVec3("lightPos", lightPos);
        terrainShader->setVec3("viewPos", camera->position);
        terrainShader->setVec3("biomeColor", glm::vec3(0.2f, 0.8f, 0.2f));
        
        // Fog parameters for distance blending to skybox
        terrainShader->setFloat("fogDensity", 0.00008f); // Exponential fog density
        terrainShader->setFloat("fogStart", 500.0f); // Distance where fog starts
        terrainShader->setVec3("fogColor", glm::vec3(0.02f, 0.02f, 0.08f)); // Dark sky color to match skybox
        terrainShader->setInt("shadowMap", 1);
        
        if (clipPlane) {
            glEnable(GL_CLIP_DISTANCE0);
            terrainShader->setVec4("clipPlane", glm::vec4(0, 1, 0, -water->getWaterLevel()));
        } else {
            terrainShader->setVec4("clipPlane", glm::vec4(0, 0, 0, 0));
        }
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        
        terrain->render(*terrainShader, *shadowShader, lightSpaceMatrix, projection * view);
        
        if (clipPlane) {
            glDisable(GL_CLIP_DISTANCE0);
        }
    }
    
    void drawDebugQuad(const glm::mat4& projection, const glm::mat4& view) {
        if (!debugQuadInit) {
            float vertices[] = {
                // positions
                0.0f, 20.0f, 0.0f,
                60.0f, 20.0f, 0.0f,
                60.0f, 20.0f, -60.0f,
                0.0f, 20.0f, -60.0f
            };
            
            unsigned int indices[] = {
                0, 1, 2,
                0, 2, 3
            };
            
            glGenVertexArrays(1, &debugVAO);
            glGenBuffers(1, &debugVBO);
            GLuint debugEBO;
            glGenBuffers(1, &debugEBO);
            
            glBindVertexArray(debugVAO);
            glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, debugEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
            
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            
            glBindVertexArray(0);
            debugQuadInit = true;
            std::cout << "Debug quad initialized" << std::endl;
        }
        
        terrainShader->use();
        glm::mat4 mvp = projection * view;
        terrainShader->setMat4("mvp", mvp);
        
        glBindVertexArray(debugVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    void run() {
        auto lastFrame = std::chrono::high_resolution_clock::now();
        
        while (running) {
            auto currentFrame = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentFrame - lastFrame).count();
            lastFrame = currentFrame;
            
            handleEvents();
            update(deltaTime);
            render();
        }
    }
    
    ~Application() {
        if (shadowMapFBO) glDeleteFramebuffers(1, &shadowMapFBO);
        if (shadowMap) glDeleteTextures(1, &shadowMap);
        
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

int main(int argc, char* argv[]) {
    Application app;
    
    if (!app.init()) {
        return -1;
    }
    
    app.run();
    
    return 0;
}
