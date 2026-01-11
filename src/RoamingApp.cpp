#include "RoamingApp.h"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>

RoamingApp::RoamingApp()
    : Application(1280, 720, "Roaming Engine - Terrain System")
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
    , m_showCube(false)
{
    setClearColor(glm::vec4(0.5f, 0.7f, 0.9f, 1.0f));
}

void RoamingApp::onInit()
{
    // Load terrain shader
    m_terrainShader.load("shaders/terrain.vert", "shaders/terrain.frag");

    // Generate terrain from heightmap
    // Try to load heightmap, if not found, create a simple test terrain
    if (!m_terrain.generate("assets/heightmaps/terrain_512.png", m_terrainSize, m_terrainMaxHeight))
    {
        std::cout << "Heightmap not found, trying smaller test heightmap..." << std::endl;
        if (!m_terrain.generate("assets/heightmaps/test.png", m_terrainSize, m_terrainMaxHeight))
        {
            std::cout << "No heightmap found. Please add a heightmap to assets/heightmaps/" << std::endl;
        }
    }

    // Load terrain textures (optional)
    m_useTerrainTextures = m_grassTexture.load("assets/textures/terrain/grass.jpg");
    if (m_useTerrainTextures)
    {
        m_rockTexture.load("assets/textures/terrain/rock.jpg");
        m_snowTexture.load("assets/textures/terrain/snow.jpg");
    }

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
        // Increase movement speed for terrain navigation
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

        // Mouse look
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

void RoamingApp::onRender()
{
    // Set wireframe mode
    if (m_wireframeMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Common matrices
    glm::mat4 projection = glm::perspective(
        glm::radians(m_camera.Zoom),
        getAspectRatio(),
        0.1f,
        1000.0f  // Extended far plane for terrain
    );
    glm::mat4 view = m_camera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    // Render terrain
    if (m_terrain.isGenerated())
    {
        m_terrainShader.use();
        m_terrainShader.setMat4("uProjection", projection);
        m_terrainShader.setMat4("uView", view);
        m_terrainShader.setMat4("uModel", model);
        
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
        
        // Position cube above terrain
        glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, m_terrainMaxHeight + 5.0f, 0.0f));
        cubeModel = glm::scale(cubeModel, glm::vec3(5.0f));
        m_cubeShader.setMat4("uModel", cubeModel);
        m_cubeShader.setBool("uUseTexture", false);
        
        m_cubeMesh.draw();
    }

    // Reset polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
        ImGui::BulletText("assets/heightmaps/terrain_512.png");
    }
    
    ImGui::Separator();
    glm::vec4 clearColor = getClearColor();
    if (ImGui::ColorEdit3("Sky Color", &clearColor.x))
    {
        setClearColor(clearColor);
    }
    
    ImGui::End();
}
