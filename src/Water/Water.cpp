#include "Water.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Water::Water()
    : m_vao(0)
    , m_vbo(0)
    , m_size(100.0f)
    , m_height(0.0f)
    , m_initialized(false)
    , m_texturesLoaded(false)
    , m_waveSpeed(0.03f)
    , m_waveStrength(0.02f)
    , m_shineDamper(20.0f)
    , m_reflectivity(0.6f)
    , m_tiling(4.0f)
    , m_waterColor(0.0f, 0.3f, 0.5f)
    , m_foamEnabled(true)
    , m_foamDepth(2.0f)
    , m_foamIntensity(0.8f)
    , m_foamColor(1.0f, 1.0f, 1.0f)
{
}

Water::~Water()
{
    release();
}

void Water::release()
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
    m_initialized = false;
}

void Water::init(float size, float height)
{
    m_size = size;
    m_height = height;

    // Load shader
    if (!m_shader.load("shaders/water.vert", "shaders/water.frag"))
    {
        std::cerr << "ERROR::WATER::FAILED_TO_LOAD_SHADER" << std::endl;
        return;
    }

    // Load textures
    m_texturesLoaded = m_dudvMap.load("assets/textures/water/dudv.png", false);
    if (m_texturesLoaded)
    {
        m_normalMap.load("assets/textures/water/normal.png", false);
    }
    else
    {
        std::cout << "Water textures not found, using procedural water" << std::endl;
    }

    // Setup mesh
    setupMesh();

    m_initialized = true;
    std::cout << "Water initialized: size=" << m_size << ", height=" << m_height << std::endl;
}

void Water::setupMesh()
{
    // Water plane vertices (centered at origin, extends from -size/2 to +size/2)
    float halfSize = m_size / 2.0f;
    float vertices[] = {
        // positions (y = 0, will be translated by model matrix)
        -halfSize, 0.0f, -halfSize,
         halfSize, 0.0f, -halfSize,
         halfSize, 0.0f,  halfSize,
        
        -halfSize, 0.0f, -halfSize,
         halfSize, 0.0f,  halfSize,
        -halfSize, 0.0f,  halfSize
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void Water::render(const glm::mat4& view, const glm::mat4& projection,
                   const glm::vec3& cameraPos, const glm::vec3& lightDir, 
                   const glm::vec3& lightColor, float lightIntensity, float time,
                   unsigned int reflectionTex, unsigned int refractionTex,
                   unsigned int depthTex,
                   const glm::vec3& fogColor, float fogDensity, bool fogEnabled)
{
    if (!m_initialized)
    {
        return;
    }

    // Enable blending for water transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shader.use();

    // Model matrix (translate to water height)
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, m_height, 0.0f));
    m_shader.setMat4("uModel", model);
    m_shader.setMat4("uView", view);
    m_shader.setMat4("uProjection", projection);

    // Camera and light
    m_shader.setVec3("uCameraPos", cameraPos);
    m_shader.setVec3("uLightDir", lightDir);
    m_shader.setVec3("uLightColor", lightColor);
    m_shader.setFloat("uLightIntensity", lightIntensity);
    
    // Fog parameters
    m_shader.setVec3("uFogColor", fogColor);
    m_shader.setFloat("uFogDensity", fogDensity);
    m_shader.setBool("uFogEnabled", fogEnabled);

    // Water parameters
    m_shader.setFloat("uTime", time);
    m_shader.setFloat("uWaveStrength", m_waveStrength);
    m_shader.setFloat("uShineDamper", m_shineDamper);
    m_shader.setFloat("uReflectivity", m_reflectivity);
    m_shader.setFloat("uTiling", m_tiling);
    m_shader.setVec3("uWaterColor", m_waterColor);
    m_shader.setBool("uUseTextures", m_texturesLoaded);
    
    // Foam parameters
    m_shader.setBool("uFoamEnabled", m_foamEnabled);
    m_shader.setFloat("uFoamDepth", m_foamDepth);
    m_shader.setFloat("uFoamIntensity", m_foamIntensity);
    m_shader.setVec3("uFoamColor", m_foamColor);

    // Bind reflection texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, reflectionTex);
    m_shader.setInt("uReflectionTexture", 0);

    // Bind refraction texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, refractionTex);
    m_shader.setInt("uRefractionTexture", 1);

    // Bind depth texture
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    m_shader.setInt("uDepthMap", 2);

    // Bind DuDv and normal maps if available
    if (m_texturesLoaded)
    {
        m_dudvMap.bind(3);
        m_shader.setInt("uDudvMap", 3);
        m_normalMap.bind(4);
        m_shader.setInt("uNormalMap", 4);
    }

    // Draw water plane
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
}
