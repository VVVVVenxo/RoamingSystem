#include "RoamingApp.h"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>

RoamingApp::RoamingApp()
    : Application(1280, 720, "Roaming Engine")
    , m_camera(glm::vec3(0.0f, 0.0f, 3.0f))
    , m_textureLoaded(false)
    , m_useTexture(false)
{
    setClearColor(glm::vec4(0.45f, 0.55f, 0.60f, 1.0f));
}

void RoamingApp::onInit()
{
    // Load shader
    m_shader.load("shaders/test.vert", "shaders/test.frag");

    // Load texture (optional)
    m_textureLoaded = m_texture.load("assets/textures/container.jpg");

    // Cube vertices: position(xyz) + color(rgb) + texcoord(uv)
    float vertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,

        // Front face
        -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,

        // Left face
        -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

        // Right face
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

        // Bottom face
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,

        // Top face
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f
    };

    m_cubeMesh.setVertices(vertices, sizeof(vertices), VertexLayout::positionColorTexture());
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

    // Camera movement (only when cursor is disabled)
    if (!isCursorEnabled())
    {
        if (isKeyPressed(GLFW_KEY_W))
            m_camera.ProcessKeyboard(FORWARD, deltaTime);
        if (isKeyPressed(GLFW_KEY_S))
            m_camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (isKeyPressed(GLFW_KEY_A))
            m_camera.ProcessKeyboard(LEFT, deltaTime);
        if (isKeyPressed(GLFW_KEY_D))
            m_camera.ProcessKeyboard(RIGHT, deltaTime);

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
    m_shader.use();

    // Set matrices
    glm::mat4 projection = glm::perspective(
        glm::radians(m_camera.Zoom),
        getAspectRatio(),
        0.1f,
        100.0f
    );
    m_shader.setMat4("uProjection", projection);

    glm::mat4 view = m_camera.GetViewMatrix();
    m_shader.setMat4("uView", view);

    glm::mat4 model = glm::mat4(1.0f);
    m_shader.setMat4("uModel", model);

    // Texture binding
    m_shader.setBool("uUseTexture", m_useTexture && m_textureLoaded);
    if (m_useTexture && m_textureLoaded)
    {
        m_texture.bind(0);
        m_shader.setInt("uTexture", 0);
    }

    m_cubeMesh.draw();
}

void RoamingApp::onImGui()
{
    ImGui::Begin("Engine Stats");
    
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", 
        m_camera.Position.x, m_camera.Position.y, m_camera.Position.z);
    
    glm::vec4 clearColor = getClearColor();
    if (ImGui::ColorEdit3("Background", &clearColor.x))
    {
        setClearColor(clearColor);
    }
    
    ImGui::Separator();
    ImGui::Checkbox("Use Texture", &m_useTexture);
    
    if (m_textureLoaded)
    {
        ImGui::Text("Texture: %dx%d", m_texture.getWidth(), m_texture.getHeight());
    }
    else
    {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No texture loaded");
    }
    
    ImGui::Separator();
    ImGui::Text("Press SPACE to toggle Mouse");
    
    ImGui::End();
}
