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

    // Prevent copying
    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;

    // Load skybox from directory containing 6 face images
    bool load(const std::string& directory, const std::string& extension = ".jpg");

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
