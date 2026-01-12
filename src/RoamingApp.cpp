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
    , m_wireframeMode(false)
    , m_waterHeight(10.0f)
    , m_enableWater(true)
    , m_time(0.0f)
    , m_groundWalkMode(false)
    , m_playerHeight(1.8f)
    , m_drawCalls(0)
    , m_showCube(false)
{
    setClearColor(glm::vec4(0.5f, 0.7f, 0.9f, 1.0f));
}

void RoamingApp::onInit()
{
    // Load skybox
    m_skybox.load("assets/skybox");
    
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
    }

    // Initialize water system
    m_waterFBOs.init(320, 180, getWidth(), getHeight());
    m_water.init(m_terrainSize, m_waterHeight);

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

    // Render skybox first (with depth write disabled)
    if (m_skybox.isLoaded())
    {
        glDepthMask(GL_FALSE);
        // Set clip plane for skybox
        // Note: Skybox shader needs to support clip plane
        m_skybox.render(view, projection);
        glDepthMask(GL_TRUE);
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
        
        // Bind textures if available
        if (m_useTerrainTextures)
        {
            m_grassTexture.bind(0);
            m_terrainShader.setInt("uGrassTexture", 0);
            m_rockTexture.bind(1);
            m_terrainShader.setInt("uRockTexture", 1);
            m_snowTexture.bind(2);
            m_terrainShader.setInt("uSnowTexture", 2);
        }
        
        m_terrain.render(m_terrainShader);
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
    }

    // Reset polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RoamingApp::onRender()
{
    glm::mat4 projection = glm::perspective(
        glm::radians(m_camera.Zoom),
        getAspectRatio(),
        0.1f,
        1000.0f
    );
    glm::mat4 view = m_camera.GetViewMatrix();

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
        m_water.render(view, projection, m_camera.Position, m_lightDir, m_time,
                       m_waterFBOs.getReflectionTexture(),
                       m_waterFBOs.getRefractionTexture(),
                       m_waterFBOs.getRefractionDepthTexture());
    }
}

void RoamingApp::onImGui()
{
    // Engine Stats
    ImGui::Begin("Engine Stats");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Camera Pos: (%.1f, %.1f, %.1f)", 
        m_camera.Position.x, m_camera.Position.y, m_camera.Position.z);
    
    if (m_terrain.isGenerated())
    {
        float terrainHeight = m_terrain.getHeightAt(m_camera.Position.x, m_camera.Position.z);
        ImGui::Text("Terrain Height: %.2f", terrainHeight);
        ImGui::Text("Height Above Terrain: %.2f", m_camera.Position.y - terrainHeight);
    }
    
    ImGui::Separator();
    ImGui::Text("Controls:");
    ImGui::BulletText("WASD - Move");
    ImGui::BulletText("SHIFT - Sprint");
    ImGui::BulletText("SPACE - Toggle Mouse");
    ImGui::BulletText("F1 - Toggle Wireframe");
    ImGui::End();

    // Terrain Settings
    ImGui::Begin("Terrain Settings");
    
    if (m_terrain.isGenerated())
    {
        ImGui::Text("Grid: %dx%d", m_terrain.getGridWidth(), m_terrain.getGridHeight());
        ImGui::Text("World Size: %.0f x %.0f", m_terrain.getSize(), m_terrain.getSize());
        
        ImGui::Separator();
        ImGui::Text("Rendering");
        ImGui::Checkbox("Wireframe Mode", &m_wireframeMode);
        ImGui::Checkbox("Show Reference Cube", &m_showCube);
        
        ImGui::Separator();
        ImGui::Text("Texturing");
        ImGui::SliderFloat("Texture Tiling", &m_textureTiling, 1.0f, 128.0f);
        ImGui::Checkbox("Use Textures", &m_useTerrainTextures);
        
        ImGui::Separator();
        ImGui::Text("Height-based Blending");
        ImGui::SliderFloat("Grass Max Height", &m_grassMaxHeight, 0.0f, 1.0f);
        ImGui::SliderFloat("Rock Max Height", &m_rockMaxHeight, 0.0f, 1.0f);
        ImGui::SliderFloat("Slope Threshold", &m_slopeThreshold, 0.0f, 1.0f);
        
        ImGui::Separator();
        ImGui::Text("Lighting");
        ImGui::SliderFloat3("Light Dir", &m_lightDir.x, -1.0f, 1.0f);
    }
    else
    {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No terrain loaded!");
        ImGui::Text("Add heightmap to:");
        ImGui::BulletText("assets/heightmaps/heightmap.png");
    }
    
    ImGui::Separator();
    glm::vec4 clearColor = getClearColor();
    if (ImGui::ColorEdit3("Sky Color", &clearColor.x))
    {
        setClearColor(clearColor);
    }
    
    ImGui::End();

    // Water Settings
    ImGui::Begin("Water Settings");
    
    ImGui::Checkbox("Enable Water", &m_enableWater);
    
    if (m_water.isInitialized())
    {
        ImGui::SliderFloat("Water Height", &m_waterHeight, 0.0f, m_terrainMaxHeight);
        m_water.setHeight(m_waterHeight);
        
        ImGui::Separator();
        ImGui::Text("Wave Parameters");
        ImGui::SliderFloat("Wave Speed", &m_water.m_waveSpeed, 0.0f, 0.2f);
        ImGui::SliderFloat("Wave Strength", &m_water.m_waveStrength, 0.0f, 0.1f);
        ImGui::SliderFloat("Tiling", &m_water.m_tiling, 1.0f, 20.0f);
        
        ImGui::Separator();
        ImGui::Text("Appearance");
        ImGui::SliderFloat("Shine Damper", &m_water.m_shineDamper, 1.0f, 100.0f);
        ImGui::SliderFloat("Reflectivity", &m_water.m_reflectivity, 0.0f, 1.0f);
        ImGui::ColorEdit3("Water Color", &m_water.m_waterColor.x);
    }
    else
    {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Water not initialized!");
    }
    
    ImGui::End();
}
