#ifndef WATER_H
#define WATER_H

#include "Core/Shader.h"
#include "Core/Texture.h"
#include <glm/glm.hpp>

class Water
{
public:
    Water();
    ~Water();

    // Initialize water plane
    void init(float size, float height);

    // Render water surface
    void render(const glm::mat4& view, const glm::mat4& projection,
                const glm::vec3& cameraPos, const glm::vec3& lightDir, 
                const glm::vec3& lightColor, float lightIntensity, float time,
                unsigned int reflectionTex, unsigned int refractionTex,
                unsigned int depthTex,
                const glm::vec3& fogColor, float fogDensity, bool fogEnabled);

    // Accessors
    float getHeight() const { return m_height; }
    void setHeight(float height) { m_height = height; }
    float getSize() const { return m_size; }
    bool isInitialized() const { return m_initialized; }

    // Public parameters for ImGui
    float m_waveSpeed;
    float m_waveStrength;
    float m_shineDamper;
    float m_reflectivity;
    float m_tiling;
    glm::vec3 m_waterColor;

private:
    Shader m_shader;
    Texture m_dudvMap;
    Texture m_normalMap;
    unsigned int m_vao;
    unsigned int m_vbo;
    float m_size;
    float m_height;
    bool m_initialized;
    bool m_texturesLoaded;

    void setupMesh();
    void release();
};

#endif
