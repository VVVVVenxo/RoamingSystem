/**
 * @file WaterFramebuffers.h
 * @brief Water reflection/refraction framebuffer manager
 * @author LuNingfang
 */

#ifndef WATER_FRAMEBUFFERS_H
#define WATER_FRAMEBUFFERS_H

#include <glad/glad.h>

class WaterFramebuffers
{
public:
    WaterFramebuffers();
    ~WaterFramebuffers();

    void init(int reflectionWidth = 320, int reflectionHeight = 180,
              int refractionWidth = 1280, int refractionHeight = 720);

    void bindReflectionFBO();
    void bindRefractionFBO();

    void unbind(int windowWidth, int windowHeight);

    unsigned int getReflectionTexture() const { return m_reflectionTexture; }
    unsigned int getRefractionTexture() const { return m_refractionTexture; }
    unsigned int getRefractionDepthTexture() const { return m_refractionDepthTexture; }

    int getReflectionWidth() const { return m_reflectionWidth; }
    int getReflectionHeight() const { return m_reflectionHeight; }
    int getRefractionWidth() const { return m_refractionWidth; }
    int getRefractionHeight() const { return m_refractionHeight; }

    bool isInitialized() const { return m_initialized; }

private:
    unsigned int m_reflectionFBO;
    unsigned int m_reflectionTexture;
    unsigned int m_reflectionDepthBuffer;
    int m_reflectionWidth;
    int m_reflectionHeight;

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
