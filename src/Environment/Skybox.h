/**
 * @file Skybox.h
 * @brief Skybox rendering class
 * @author LuNingfang
 */

#ifndef SKYBOX_H
#define SKYBOX_H

#include "Core/Cubemap.h"
#include "Core/Shader.h"
#include <glm/glm.hpp>
#include <string>

class Skybox
{
public:
    Skybox();
    ~Skybox();

    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;

    /**
     * @brief Load skybox cubemap from directory
     * @param directory Directory containing 6 face images
     * @param extension File extension (e.g., ".jpg", ".png")
     * @return True if loaded successfully
     */
    bool load(const std::string& directory, const std::string& extension = ".jpg");

    /**
     * @brief Render skybox
     * @param view View matrix (translation removed)
     * @param projection Projection matrix
     * @param skyColor Dynamic sky color (from day/night cycle)
     * @param blendFactor How much to blend cubemap with dynamic color
     */
    void render(const glm::mat4& view, const glm::mat4& projection,
                const glm::vec3& skyColor, float blendFactor);

    bool isLoaded() const { return m_cubemap.isLoaded(); }

private:
    Cubemap m_cubemap;
    Shader m_shader;
    unsigned int m_vao;
    unsigned int m_vbo;

    void setupMesh();
    void release();
};

#endif
