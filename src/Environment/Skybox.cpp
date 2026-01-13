#include "Skybox.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Skybox cube vertices (inward facing)
static const float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

Skybox::Skybox()
    : m_vao(0)
    , m_vbo(0)
{
}

Skybox::~Skybox()
{
    release();
}

void Skybox::release()
{
    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_vbo != 0)
    {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
}

void Skybox::setupMesh()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

bool Skybox::load(const std::string& directory, const std::string& extension)
{
    // Load cubemap textures
    if (!m_cubemap.loadFromDirectory(directory, extension))
    {
        std::cerr << "ERROR::SKYBOX::FAILED_TO_LOAD_CUBEMAP" << std::endl;
        return false;
    }

    // Load skybox shader
    if (!m_shader.load("shaders/skybox.vert", "shaders/skybox.frag"))
    {
        std::cerr << "ERROR::SKYBOX::FAILED_TO_LOAD_SHADER" << std::endl;
        return false;
    }

    // Setup mesh
    setupMesh();

    std::cout << "Skybox loaded successfully" << std::endl;
    return true;
}

void Skybox::render(const glm::mat4& view, const glm::mat4& projection,
                    const glm::vec3& skyColor, float blendFactor)
{
    if (!m_cubemap.isLoaded())
    {
        return;
    }

    GLint oldDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFunc);
    glDepthFunc(GL_LEQUAL);

    m_shader.use();

    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
    m_shader.setMat4("uView", viewNoTranslation);
    m_shader.setMat4("uProjection", projection);
    m_shader.setVec4("uClipPlane", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    m_shader.setVec3("uSkyColor", skyColor);
    m_shader.setFloat("uBlendFactor", blendFactor);

    m_cubemap.bind(0);
    m_shader.setInt("uSkybox", 0);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(oldDepthFunc);
}
