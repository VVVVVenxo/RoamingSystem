#ifndef WATER_FRAMEBUFFERS_H
#define WATER_FRAMEBUFFERS_H

#include <glad/glad.h>

class WaterFramebuffers
{
public:
    WaterFramebuffers();
    ~WaterFramebuffers();

    // Initialize FBOs with specified dimensions
    void init(int reflectionWidth = 320, int reflectionHeight = 180,
              int refractionWidth = 1280, int refractionHeight = 720);

    // Bind reflection/refraction FBOs
    void bindReflectionFBO();
    void bindRefractionFBO();

    // Unbind and restore default framebuffer
    void unbind(int windowWidth, int windowHeight);

    // Texture getters
    unsigned int getReflectionTexture() const { return m_reflectionTexture; }
    unsigned int getRefractionTexture() const { return m_refractionTexture; }
    unsigned int getRefractionDepthTexture() const { return m_refractionDepthTexture; }

    // Dimension getters
    int getReflectionWidth() const { return m_reflectionWidth; }
    int getReflectionHeight() const { return m_reflectionHeight; }
    int getRefractionWidth() const { return m_refractionWidth; }
    int getRefractionHeight() const { return m_refractionHeight; }

    bool isInitialized() const { return m_initialized; }

private:
    // Reflection FBO
    unsigned int m_reflectionFBO;
    unsigned int m_reflectionTexture;
    unsigned int m_reflectionDepthBuffer;
    int m_reflectionWidth;
    int m_reflectionHeight;

    // Refraction FBO
    unsigned int m_refractionFBO;
    unsigned int m_refractionTexture;
    unsigned int m_refractionDepthTexture;
    int m_refractionWidth;
    int m_refractionHeight;

    bool m_initialized;

    void createReflectionFBO();
    void createRefractionFBO();
    void release();
};

#endif
