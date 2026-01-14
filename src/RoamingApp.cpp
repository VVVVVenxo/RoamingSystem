#include "RoamingApp.h"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>

RoamingApp::RoamingApp()
    : Application(1920, 1080, "OpenGL Terrain Roaming System")
    , m_camera(glm::vec3(0.0f, 30.0f, 50.0f))
    , m_terrainSize(256.0f)
    , m_terrainMaxHeight(50.0f)
    , m_textureTiling(32.0f)
    , m_lightDir(glm::normalize(glm::vec3(-0.5f, -1.0f, -0.3f)))
    , m_grassMaxHeight(0.35f)
    , m_rockMaxHeight(0.7f)
    , m_slopeThreshold(0.4f)
    , m_useTerrainTextures(false)
    , m_useNormalMaps(false)
    , m_normalMapStrength(1.0f)
    , m_wireframeMode(false)
    , m_waterHeight(10.0f)
    , m_enableWater(true)
    , m_time(0.0f)
    , m_groundWalkMode(false)
    , m_playerHeight(1.8f)
    , m_enableFog(true)
    , m_fogDensity(0.003f)
    , m_enableSSAO(true)
    , m_ssaoRadius(0.5f)
    , m_ssaoBias(0.025f)
    , m_ssaoIntensity(1.0f)
    , m_ssaoKernelSize(32)
    , m_drawCalls(0)
    , m_showCube(false)
{
    setClearColor(glm::vec4(0.5f, 0.7f, 0.9f, 1.0f));
}

void RoamingApp::onInit()
{
    // Try to load saved settings
    loadSettings();
    
    // Load skybox
    m_skybox.load("assets/skybox", ".png");
    
    // Load terrain shader
    m_terrainShader.load("shaders/terrain.vert", "shaders/terrain.frag");

    // Generate terrain from heightmap
    if (!m_terrain.generate("assets/heightmaps/heightmap.png", m_terrainSize, m_terrainMaxHeight))
    {
        std::cout << "Heightmap not found, trying smaller test heightmap..." << std::endl;
        if (!m_terrain.generate("assets/heightmaps/test.png", m_terrainSize, m_terrainMaxHeight))
        {
            std::cout << "No heightmap found. Please add a heightmap to assets/heightmaps/" << std::endl;
        }
    }

    // Load terrain textures (optional)
    m_useTerrainTextures = m_grassTexture.load("assets/textures/terrain/Ground037_1K-PNG_Color.png");
    if (m_useTerrainTextures)
    {
        m_rockTexture.load("assets/textures/terrain/Rocks001_1K-PNG_Color.png");
        m_snowTexture.load("assets/textures/terrain/Snow010A_1K-PNG_Color.png");
        
        // Load normal maps
        m_useNormalMaps = m_grassNormalMap.load("assets/textures/terrain/Ground037_1K-PNG_NormalGL.png");
        if (m_useNormalMaps)
        {
            m_rockNormalMap.load("assets/textures/terrain/Rocks001_1K-PNG_NormalGL.png");
            m_snowNormalMap.load("assets/textures/terrain/Snow010A_1K-PNG_NormalGL.png");
        }
    }

    // Initialize water system
    m_waterFBOs.init(320, 180, getWidth(), getHeight());
    m_water.init(m_terrainSize, m_waterHeight);
    
    // Initialize SSAO
    m_ssao.init(getWidth(), getHeight());
    m_gbufferShader.load("shaders/gbuffer.vert", "shaders/gbuffer.frag");

    // Load cube shader and mesh for reference
    m_cubeShader.load("shaders/test.vert", "shaders/test.frag");
    
    // Simple cube vertices
    float cubeVertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
        // Front face
        -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
        // Left face
        -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
        // Right face
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,
        // Bottom face
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f,
        // Top face
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f
    };
    m_cubeMesh.setVertices(cubeVertices, sizeof(cubeVertices), VertexLayout::positionColorTexture());
}

void RoamingApp::onUpdate(float deltaTime)
{
    m_time += deltaTime;
    processInput(deltaTime);
    
    // Update lighting (for day/night cycle)
    m_lighting.update(deltaTime);
    
    // Update sky color based on time of day
    glm::vec3 skyColor = m_lighting.getSkyColor();
    setClearColor(glm::vec4(skyColor, 1.0f));
    
    // Update light direction from lighting system
    m_lightDir = -m_lighting.getSunDirection();
    
    // Ground walk mode: constrain camera to terrain surface
    if (m_groundWalkMode && m_terrain.isGenerated())
    {
        float terrainHeight = m_terrain.getHeightAt(m_camera.Position.x, m_camera.Position.z);
        m_camera.Position.y = terrainHeight + m_playerHeight;
    }
}

void RoamingApp::processInput(float deltaTime)
{
    // Close window on ESC
    if (isKeyPressed(GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(m_window, true);
    }

    // Toggle cursor on SPACE
    static bool spacePressed = false;
    if (isKeyPressed(GLFW_KEY_SPACE) && !spacePressed)
    {
        setCursorEnabled(!isCursorEnabled());
        spacePressed = true;
    }
    if (!isKeyPressed(GLFW_KEY_SPACE))
    {
        spacePressed = false;
    }

    // Toggle wireframe on F1
    static bool f1Pressed = false;
    if (isKeyPressed(GLFW_KEY_F1) && !f1Pressed)
    {
        m_wireframeMode = !m_wireframeMode;
        f1Pressed = true;
    }
    if (!isKeyPressed(GLFW_KEY_F1))
    {
        f1Pressed = false;
    }

    // Camera movement (only when cursor is disabled)
    if (!isCursorEnabled())
    {
        float speedMultiplier = 1.0f;
        if (isKeyPressed(GLFW_KEY_LEFT_SHIFT))
        {
            speedMultiplier = 3.0f;
        }

        if (isKeyPressed(GLFW_KEY_W))
            m_camera.ProcessKeyboard(FORWARD, deltaTime * speedMultiplier);
        if (isKeyPressed(GLFW_KEY_S))
            m_camera.ProcessKeyboard(BACKWARD, deltaTime * speedMultiplier);
        if (isKeyPressed(GLFW_KEY_A))
            m_camera.ProcessKeyboard(LEFT, deltaTime * speedMultiplier);
        if (isKeyPressed(GLFW_KEY_D))
            m_camera.ProcessKeyboard(RIGHT, deltaTime * speedMultiplier);

        glm::vec2 delta = getMouseDelta();
        m_camera.ProcessMouseMovement(delta.x, -delta.y);
    }
}

void RoamingApp::onScroll(float yoffset)
{
    if (!isCursorEnabled())
    {
        m_camera.ProcessMouseScroll(yoffset);
    }
}

void RoamingApp::renderScene(const glm::mat4& view, const glm::mat4& projection, const glm::vec4& clipPlane)
{
    glm::mat4 model = glm::mat4(1.0f);

    // Render skybox
    if (m_skybox.isLoaded())
    {
        glDepthMask(GL_FALSE);
        float sunHeight = m_lighting.getSunDirection().y;
        float blendFactor = (sunHeight < 0.3f) ? 0.5f * (1.0f - sunHeight / 0.3f) : 0.0f;
        m_skybox.render(view, projection, m_lighting.getSkyColor(), blendFactor);
        glDepthMask(GL_TRUE);
        m_drawCalls++;
    }

    // Set wireframe mode for terrain
    if (m_wireframeMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Render terrain
    if (m_terrain.isGenerated())
    {
        m_terrainShader.use();
        m_terrainShader.setMat4("uProjection", projection);
        m_terrainShader.setMat4("uView", view);
        m_terrainShader.setMat4("uModel", model);
        m_terrainShader.setVec4("uClipPlane", clipPlane);
        
        // Terrain parameters
        m_terrainShader.setFloat("uMaxHeight", m_terrainMaxHeight);
        m_terrainShader.setFloat("uTextureTiling", m_textureTiling);
        m_terrainShader.setVec3("uLightDir", m_lightDir);
        m_terrainShader.setFloat("uGrassMaxHeight", m_grassMaxHeight);
        m_terrainShader.setFloat("uRockMaxHeight", m_rockMaxHeight);
        m_terrainShader.setFloat("uSlopeThreshold", m_slopeThreshold);
        m_terrainShader.setBool("uUseTextures", m_useTerrainTextures);
        
        // Dynamic lighting parameters
        m_terrainShader.setVec3("uLightColor", m_lighting.getSunColor());
        m_terrainShader.setVec3("uAmbientColor", m_lighting.getAmbientColor());
        m_terrainShader.setFloat("uLightIntensity", m_lighting.getSunIntensity());
        
        // Fog parameters
        m_terrainShader.setVec3("uCameraPos", m_camera.Position);
        m_terrainShader.setVec3("uFogColor", m_lighting.getFogColor());
        m_terrainShader.setFloat("uFogDensity", m_fogDensity);
        m_terrainShader.setBool("uFogEnabled", m_enableFog);
        
        // SSAO parameters
        m_terrainShader.setBool("uSSAOEnabled", m_enableSSAO && m_ssao.isInitialized());
        m_terrainShader.setFloat("uSSAOIntensity", m_ssaoIntensity);
        if (m_enableSSAO && m_ssao.isInitialized())
        {
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, m_ssao.getSSAOTexture());
            m_terrainShader.setInt("uSSAOTexture", 6);
        }
        
        // Bind textures if available
        if (m_useTerrainTextures)
        {
            m_grassTexture.bind(0);
            m_terrainShader.setInt("uGrassTexture", 0);
            m_rockTexture.bind(1);
            m_terrainShader.setInt("uRockTexture", 1);
            m_snowTexture.bind(2);
            m_terrainShader.setInt("uSnowTexture", 2);
            
            // Bind normal maps
            m_terrainShader.setBool("uUseNormalMaps", m_useNormalMaps);
            m_terrainShader.setFloat("uNormalMapStrength", m_normalMapStrength);
            if (m_useNormalMaps)
            {
                m_grassNormalMap.bind(3);
                m_terrainShader.setInt("uGrassNormalMap", 3);
                m_rockNormalMap.bind(4);
                m_terrainShader.setInt("uRockNormalMap", 4);
                m_snowNormalMap.bind(5);
                m_terrainShader.setInt("uSnowNormalMap", 5);
            }
        }
        else
        {
            m_terrainShader.setBool("uUseNormalMaps", false);
        }
        
        glm::mat4 vp = projection * view;
        m_terrain.render(m_terrainShader, m_camera.Position, vp);
        m_drawCalls += m_terrain.getVisibleChunks();
    }

    // Render reference cube
    if (m_showCube)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        m_cubeShader.use();
        m_cubeShader.setMat4("uProjection", projection);
        m_cubeShader.setMat4("uView", view);
        
        glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, m_terrainMaxHeight + 5.0f, 0.0f));
        cubeModel = glm::scale(cubeModel, glm::vec3(5.0f));
        m_cubeShader.setMat4("uModel", cubeModel);
        m_cubeShader.setBool("uUseTexture", false);
        
        m_cubeMesh.draw();
        m_drawCalls++;
    }

    // Reset polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RoamingApp::onRender()
{
    // Reset draw call counter
    m_drawCalls = 0;
    
    glm::mat4 projection = glm::perspective(
        glm::radians(m_camera.Zoom),
        getAspectRatio(),
        0.1f,
        1000.0f
    );
    glm::mat4 view = m_camera.GetViewMatrix();

    // SSAO Pass: Render G-Buffer and calculate SSAO
    if (m_enableSSAO && m_ssao.isInitialized())
    {
        // Update SSAO parameters
        m_ssao.m_enabled = m_enableSSAO;
        m_ssao.m_radius = m_ssaoRadius;
        m_ssao.m_bias = m_ssaoBias;
        m_ssao.m_intensity = m_ssaoIntensity;
        m_ssao.m_kernelSize = m_ssaoKernelSize;
        
        // 1. Render scene to G-Buffer
        m_ssao.bindGBuffer();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Render terrain to G-Buffer
        if (m_terrain.isGenerated())
        {
            glm::mat4 model = glm::mat4(1.0f);
            m_gbufferShader.use();
            m_gbufferShader.setMat4("uProjection", projection);
            m_gbufferShader.setMat4("uView", view);
            m_gbufferShader.setMat4("uModel", model);
            
            glm::mat4 vp = projection * view;
            m_terrain.render(m_gbufferShader, m_camera.Position, vp);
        }
        
        // 2. Calculate SSAO
        m_ssao.renderSSAO(projection, view);
        
        // 3. Blur SSAO
        m_ssao.renderBlur();
        
        m_ssao.unbind(getWidth(), getHeight());
    }

    if (m_enableWater && m_water.isInitialized())
    {
        glEnable(GL_CLIP_DISTANCE0);

        // 1. Render reflection (camera below water, looking up)
        m_waterFBOs.bindReflectionFBO();
        
        float distance = 2.0f * (m_camera.Position.y - m_waterHeight);
        glm::vec3 reflectedCameraPos = m_camera.Position;
        reflectedCameraPos.y -= distance;
        
        // Create reflected view matrix
        Camera reflectedCamera = m_camera;
        reflectedCamera.Position = reflectedCameraPos;
        reflectedCamera.Pitch = -reflectedCamera.Pitch;
        reflectedCamera.updateCameraVectors();
        glm::mat4 reflectedView = reflectedCamera.GetViewMatrix();
        
        // Clip everything below water surface
        renderScene(reflectedView, projection, glm::vec4(0.0f, 1.0f, 0.0f, -m_waterHeight + 0.1f));

        // 2. Render refraction (normal view, clip above water)
        m_waterFBOs.bindRefractionFBO();
        
        // Clip everything above water surface
        renderScene(view, projection, glm::vec4(0.0f, -1.0f, 0.0f, m_waterHeight + 0.1f));

        // 3. Unbind FBOs and render normal scene
        m_waterFBOs.unbind(getWidth(), getHeight());
        glDisable(GL_CLIP_DISTANCE0);
    }

    // Clear the default framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render scene normally (no clipping)
    renderScene(view, projection, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

    // 4. Render water surface
    if (m_enableWater && m_water.isInitialized())
    {
        m_water.render(view, projection, m_camera.Position, m_lightDir,
                       m_lighting.getSunColor(), m_lighting.getSunIntensity(), m_time,
                       m_waterFBOs.getReflectionTexture(),
                       m_waterFBOs.getRefractionTexture(),
                       m_waterFBOs.getRefractionDepthTexture(),
                       m_lighting.getFogColor(), m_fogDensity, m_enableFog);
        m_drawCalls++;
    }
}

void RoamingApp::onImGui()
{
    float fps = ImGui::GetIO().Framerate;
    
    // Performance Panel
    ImGui::Begin("Performance");
    ImGui::Text("FPS: %.1f (%.2f ms)", fps, 1000.0f / fps);
    ImGui::Separator();
    
    if (m_terrain.isGenerated())
    {
        ImGui::Text("Chunks: %d / %d visible", m_terrain.getVisibleChunks(), m_terrain.getTotalChunks());
        ImGui::Text("Culled: %d (%.1f%%)", m_terrain.getCulledChunks(), 
            m_terrain.getTotalChunks() > 0 ? 100.0f * m_terrain.getCulledChunks() / m_terrain.getTotalChunks() : 0.0f);
        ImGui::Text("Triangles: %d", m_terrain.getTriangleCount());
    }
    ImGui::Text("Draw Calls: %d", m_drawCalls);
    
    ImGui::Separator();
    ImGui::Text("OpenGL: %s", glGetString(GL_VERSION));
    ImGui::Text("GPU: %s", glGetString(GL_RENDERER));
    ImGui::End();

    // Main Scene Editor Panel
    ImGui::Begin("Scene Editor");
    
    // Camera Section
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Position: (%.1f, %.1f, %.1f)", 
            m_camera.Position.x, m_camera.Position.y, m_camera.Position.z);
        
        if (m_terrain.isGenerated())
        {
            float terrainHeight = m_terrain.getHeightAt(m_camera.Position.x, m_camera.Position.z);
            ImGui::Text("Terrain Height: %.2f", terrainHeight);
            ImGui::Text("Height Above Terrain: %.2f", m_camera.Position.y - terrainHeight);
        }
        
        ImGui::Checkbox("Ground Walk Mode", &m_groundWalkMode);
        if (m_groundWalkMode)
        {
            ImGui::SliderFloat("Player Height", &m_playerHeight, 0.5f, 5.0f);
        }
        ImGui::SliderFloat("Move Speed", &m_camera.MovementSpeed, 1.0f, 50.0f);
        ImGui::SliderFloat("Mouse Sensitivity", &m_camera.MouseSensitivity, 0.01f, 0.5f);
    }
    
    // Terrain Section
    if (ImGui::CollapsingHeader("Terrain", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (m_terrain.isGenerated())
        {
            ImGui::Text("Grid: %dx%d", m_terrain.getGridWidth(), m_terrain.getGridHeight());
            ImGui::Text("World Size: %.0f x %.0f", m_terrain.getSize(), m_terrain.getSize());
            
            ImGui::Checkbox("Wireframe Mode", &m_wireframeMode);
            ImGui::Checkbox("Show Reference Cube", &m_showCube);
            
            ImGui::Separator();
            ImGui::Text("Texturing");
            ImGui::SliderFloat("Texture Tiling", &m_textureTiling, 1.0f, 128.0f);
            ImGui::Checkbox("Use Textures", &m_useTerrainTextures);
            if (m_useTerrainTextures)
            {
                ImGui::Checkbox("Use Normal Maps", &m_useNormalMaps);
                if (m_useNormalMaps)
                {
                    ImGui::SliderFloat("Normal Strength", &m_normalMapStrength, 0.0f, 2.0f);
                }
            }
            
            ImGui::Separator();
            ImGui::Text("Height Blending");
            ImGui::SliderFloat("Grass Max", &m_grassMaxHeight, 0.0f, 1.0f);
            ImGui::SliderFloat("Rock Max", &m_rockMaxHeight, 0.0f, 1.0f);
            ImGui::SliderFloat("Slope Threshold", &m_slopeThreshold, 0.0f, 1.0f);
            
            ImGui::Separator();
            ImGui::Text("LOD & Culling");
            auto& ct = m_terrain.getChunkedTerrain();
            ImGui::Checkbox("Enable Frustum Culling", &ct.m_enableFrustumCulling);
            ImGui::Checkbox("Enable LOD", &ct.m_enableLOD);
            if (ct.m_enableLOD)
            {
                ImGui::SliderFloat("LOD0 Distance", &ct.m_lodDistances[0], 50.0f, 200.0f);
                ImGui::SliderFloat("LOD1 Distance", &ct.m_lodDistances[1], 100.0f, 400.0f);
                ImGui::SliderFloat("LOD2 Distance", &ct.m_lodDistances[2], 200.0f, 600.0f);
                ImGui::SliderFloat("LOD3 Distance", &ct.m_lodDistances[3], 400.0f, 1000.0f);
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No terrain loaded!");
            ImGui::Text("Add heightmap to:");
            ImGui::BulletText("assets/heightmaps/heightmap.png");
        }
    }
    
    // Water Section
    if (ImGui::CollapsingHeader("Water"))
    {
        ImGui::Checkbox("Enable Water", &m_enableWater);
        
        if (m_water.isInitialized() && m_enableWater)
        {
            ImGui::SliderFloat("Water Height", &m_waterHeight, 0.0f, m_terrainMaxHeight);
            m_water.setHeight(m_waterHeight);
            
            ImGui::Separator();
            ImGui::Text("Waves");
            ImGui::SliderFloat("Wave Speed", &m_water.m_waveSpeed, 0.0f, 0.2f);
            ImGui::SliderFloat("Wave Strength", &m_water.m_waveStrength, 0.0f, 0.1f);
            ImGui::SliderFloat("Tiling", &m_water.m_tiling, 1.0f, 20.0f);
            
            ImGui::Separator();
            ImGui::Text("Appearance");
            ImGui::SliderFloat("Shine Damper", &m_water.m_shineDamper, 1.0f, 100.0f);
            ImGui::SliderFloat("Reflectivity", &m_water.m_reflectivity, 0.0f, 1.0f);
            ImGui::ColorEdit3("Color", &m_water.m_waterColor.x);
            
            ImGui::Separator();
            ImGui::Text("Shore Foam");
            ImGui::Checkbox("Enable Foam", &m_water.m_foamEnabled);
            if (m_water.m_foamEnabled)
            {
                ImGui::SliderFloat("Foam Depth", &m_water.m_foamDepth, 0.5f, 10.0f);
                ImGui::SliderFloat("Foam Intensity", &m_water.m_foamIntensity, 0.0f, 1.0f);
                ImGui::ColorEdit3("Foam Color", &m_water.m_foamColor.x);
            }
        }
    }
    
    // Lighting Section
    if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // Time of day control
        ImGui::Text("Day/Night Cycle");
        ImGui::SliderFloat("Time of Day", &m_lighting.m_timeOfDay, 0.0f, 24.0f, "%.1f h");
        
        // Time presets
        if (ImGui::Button("Dawn")) m_lighting.m_timeOfDay = 6.0f;
        ImGui::SameLine();
        if (ImGui::Button("Noon")) m_lighting.m_timeOfDay = 12.0f;
        ImGui::SameLine();
        if (ImGui::Button("Dusk")) m_lighting.m_timeOfDay = 18.0f;
        ImGui::SameLine();
        if (ImGui::Button("Night")) m_lighting.m_timeOfDay = 0.0f;
        
        // Auto time advance
        ImGui::Checkbox("Auto Advance", &m_lighting.m_autoAdvance);
        if (m_lighting.m_autoAdvance)
        {
            ImGui::SliderFloat("Day Speed", &m_lighting.m_daySpeed, 0.01f, 2.0f, "%.2f h/s");
        }
        
        // Display sun info
        ImGui::Separator();
        glm::vec3 sunDir = m_lighting.getSunDirection();
        ImGui::Text("Sun Height: %.2f", sunDir.y);
        ImGui::Text("Sun Intensity: %.2f", m_lighting.getSunIntensity());
        
        glm::vec3 sunColor = m_lighting.getSunColor();
        ImGui::ColorEdit3("Sun Color", &sunColor.x, ImGuiColorEditFlags_NoInputs);
    }
    
    // Fog Section
    if (ImGui::CollapsingHeader("Fog"))
    {
        ImGui::Checkbox("Enable Fog", &m_enableFog);
        
        if (m_enableFog)
        {
            ImGui::SliderFloat("Fog Density", &m_fogDensity, 0.0f, 0.02f, "%.4f");
            
            glm::vec3 fogColor = m_lighting.getFogColor();
            ImGui::ColorEdit3("Fog Color", &fogColor.x, ImGuiColorEditFlags_NoInputs);
            ImGui::Text("(Color follows time of day)");
        }
    }
    
    // SSAO Section
    if (ImGui::CollapsingHeader("SSAO"))
    {
        ImGui::Checkbox("Enable SSAO", &m_enableSSAO);
        
        if (m_enableSSAO && m_ssao.isInitialized())
        {
            ImGui::SliderFloat("Radius", &m_ssaoRadius, 0.1f, 2.0f);
            ImGui::SliderFloat("Bias", &m_ssaoBias, 0.0f, 0.1f, "%.4f");
            ImGui::SliderFloat("Intensity", &m_ssaoIntensity, 0.0f, 2.0f);
            ImGui::SliderInt("Kernel Size", &m_ssaoKernelSize, 8, 64);
        }
        else if (!m_ssao.isInitialized())
        {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "SSAO not initialized");
        }
    }
    
    // Settings buttons
    ImGui::Separator();
    if (ImGui::Button("Save Settings"))
    {
        saveSettings();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Settings"))
    {
        loadSettings();
    }
    
    ImGui::End();

    // Help Panel (collapsed by default)
    ImGui::Begin("Controls");
    ImGui::BulletText("WASD - Move");
    ImGui::BulletText("SHIFT - Sprint");
    ImGui::BulletText("Mouse - Look around");
    ImGui::BulletText("Scroll - Zoom");
    ImGui::BulletText("SPACE - Toggle cursor");
    ImGui::BulletText("F1 - Toggle wireframe");
    ImGui::BulletText("ESC - Exit");
    ImGui::End();
}

void RoamingApp::saveSettings()
{
    SceneSettings settings = gatherSettings();
    SceneSettingsManager::save(settings, SceneSettingsManager::getDefaultPath());
}

void RoamingApp::loadSettings()
{
    SceneSettings settings;
    if (SceneSettingsManager::load(settings, SceneSettingsManager::getDefaultPath()))
    {
        applySettings(settings);
    }
}

SceneSettings RoamingApp::gatherSettings()
{
    SceneSettings settings;
    
    // Terrain
    settings.textureTiling = m_textureTiling;
    settings.grassMaxHeight = m_grassMaxHeight;
    settings.rockMaxHeight = m_rockMaxHeight;
    settings.slopeThreshold = m_slopeThreshold;
    settings.useTerrainTextures = m_useTerrainTextures;
    
    // Water
    settings.waterHeight = m_waterHeight;
    settings.waveSpeed = m_water.m_waveSpeed;
    settings.waveStrength = m_water.m_waveStrength;
    settings.waterTiling = m_water.m_tiling;
    settings.shineDamper = m_water.m_shineDamper;
    settings.reflectivity = m_water.m_reflectivity;
    settings.waterColor = m_water.m_waterColor;
    settings.enableWater = m_enableWater;
    
    // Lighting
    settings.timeOfDay = m_lighting.m_timeOfDay;
    settings.daySpeed = m_lighting.m_daySpeed;
    settings.autoAdvance = m_lighting.m_autoAdvance;
    
    // Fog
    settings.enableFog = m_enableFog;
    settings.fogDensity = m_fogDensity;
    
    // SSAO
    settings.enableSSAO = m_enableSSAO;
    settings.ssaoRadius = m_ssaoRadius;
    settings.ssaoBias = m_ssaoBias;
    settings.ssaoIntensity = m_ssaoIntensity;
    settings.ssaoKernelSize = m_ssaoKernelSize;
    
    // Camera
    settings.cameraPos = m_camera.Position;
    settings.cameraYaw = m_camera.Yaw;
    settings.cameraPitch = m_camera.Pitch;
    settings.moveSpeed = m_camera.MovementSpeed;
    settings.mouseSensitivity = m_camera.MouseSensitivity;
    settings.groundWalkMode = m_groundWalkMode;
    settings.playerHeight = m_playerHeight;
    
    // Display
    settings.wireframeMode = m_wireframeMode;
    settings.showCube = m_showCube;
    
    return settings;
}

void RoamingApp::applySettings(const SceneSettings& settings)
{
    // Terrain
    m_textureTiling = settings.textureTiling;
    m_grassMaxHeight = settings.grassMaxHeight;
    m_rockMaxHeight = settings.rockMaxHeight;
    m_slopeThreshold = settings.slopeThreshold;
    m_useTerrainTextures = settings.useTerrainTextures;
    
    // Water
    m_waterHeight = settings.waterHeight;
    m_water.setHeight(m_waterHeight);
    m_water.m_waveSpeed = settings.waveSpeed;
    m_water.m_waveStrength = settings.waveStrength;
    m_water.m_tiling = settings.waterTiling;
    m_water.m_shineDamper = settings.shineDamper;
    m_water.m_reflectivity = settings.reflectivity;
    m_water.m_waterColor = settings.waterColor;
    m_enableWater = settings.enableWater;
    
    // Lighting
    m_lighting.m_timeOfDay = settings.timeOfDay;
    m_lighting.m_daySpeed = settings.daySpeed;
    m_lighting.m_autoAdvance = settings.autoAdvance;
    
    // Fog
    m_enableFog = settings.enableFog;
    m_fogDensity = settings.fogDensity;
    
    // SSAO
    m_enableSSAO = settings.enableSSAO;
    m_ssaoRadius = settings.ssaoRadius;
    m_ssaoBias = settings.ssaoBias;
    m_ssaoIntensity = settings.ssaoIntensity;
    m_ssaoKernelSize = settings.ssaoKernelSize;
    
    // Camera
    m_camera.Position = settings.cameraPos;
    m_camera.Yaw = settings.cameraYaw;
    m_camera.Pitch = settings.cameraPitch;
    m_camera.MovementSpeed = settings.moveSpeed;
    m_camera.MouseSensitivity = settings.mouseSensitivity;
    m_camera.updateCameraVectors();
    m_groundWalkMode = settings.groundWalkMode;
    m_playerHeight = settings.playerHeight;
    
    // Display
    m_wireframeMode = settings.wireframeMode;
    m_showCube = settings.showCube;
}
