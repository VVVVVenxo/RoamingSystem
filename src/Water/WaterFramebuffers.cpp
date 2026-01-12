#include "WaterFramebuffers.h"
#include <iostream>

WaterFramebuffers::WaterFramebuffers()
    : m_reflectionFBO(0)
    , m_reflectionTexture(0)
    , m_reflectionDepthBuffer(0)
    , m_reflectionWidth(320)
    , m_reflectionHeight(180)
    , m_refractionFBO(0)
    , m_refractionTexture(0)
    , m_refractionDepthTexture(0)
    , m_refractionWidth(1280)
    , m_refractionHeight(720)
    , m_initialized(false)
{
}

WaterFramebuffers::~WaterFramebuffers()
{
    release();
}

void WaterFramebuffers::release()
{
    if (m_reflectionFBO != 0)
    {
        glDeleteFramebuffers(1, &m_reflectionFBO);
        m_reflectionFBO = 0;
    }
    if (m_reflectionTexture != 0)
    {
        glDeleteTextures(1, &m_reflectionTexture);
        m_reflectionTexture = 0;
    }
    if (m_reflectionDepthBuffer != 0)
    {
        glDeleteRenderbuffers(1, &m_reflectionDepthBuffer);
        m_reflectionDepthBuffer = 0;
    }

    if (m_refractionFBO != 0)
    {
        glDeleteFramebuffers(1, &m_refractionFBO);
        m_refractionFBO = 0;
    }
    if (m_refractionTexture != 0)
    {
        glDeleteTextures(1, &m_refractionTexture);
        m_refractionTexture = 0;
    }
    if (m_refractionDepthTexture != 0)
    {
        glDeleteTextures(1, &m_refractionDepthTexture);
        m_refractionDepthTexture = 0;
    }

    m_initialized = false;
}

void WaterFramebuffers::init(int reflectionWidth, int reflectionHeight,
                              int refractionWidth, int refractionHeight)
{
    release();

    m_reflectionWidth = reflectionWidth;
    m_reflectionHeight = reflectionHeight;
    m_refractionWidth = refractionWidth;
    m_refractionHeight = refractionHeight;

    createReflectionFBO();
    createRefractionFBO();

    m_initialized = true;
    std::cout << "WaterFramebuffers initialized: Reflection(" << m_reflectionWidth << "x" << m_reflectionHeight
              << "), Refraction(" << m_refractionWidth << "x" << m_refractionHeight << ")" << std::endl;
}

void WaterFramebuffers::createReflectionFBO()
{
    // Create FBO
    glGenFramebuffers(1, &m_reflectionFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_reflectionFBO);

    // Create color texture
    glGenTextures(1, &m_reflectionTexture);
    glBindTexture(GL_TEXTURE_2D, m_reflectionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_reflectionWidth, m_reflectionHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_reflectionTexture, 0);

    // Create depth renderbuffer
    glGenRenderbuffers(1, &m_reflectionDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_reflectionDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_reflectionWidth, m_reflectionHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_reflectionDepthBuffer);

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "ERROR::WATER_FBO::REFLECTION_FRAMEBUFFER_INCOMPLETE" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WaterFramebuffers::createRefractionFBO()
{
    // Create FBO
    glGenFramebuffers(1, &m_refractionFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_refractionFBO);

    // Create color texture
    glGenTextures(1, &m_refractionTexture);
    glBindTexture(GL_TEXTURE_2D, m_refractionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_refractionWidth, m_refractionHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_refractionTexture, 0);

    // Create depth texture (needed for soft edges)
    glGenTextures(1, &m_refractionDepthTexture);
    glBindTexture(GL_TEXTURE_2D, m_refractionDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, m_refractionWidth, m_refractionHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_refractionDepthTexture, 0);

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "ERROR::WATER_FBO::REFRACTION_FRAMEBUFFER_INCOMPLETE" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WaterFramebuffers::bindReflectionFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_reflectionFBO);
    glViewport(0, 0, m_reflectionWidth, m_reflectionHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void WaterFramebuffers::bindRefractionFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_refractionFBO);
    glViewport(0, 0, m_refractionWidth, m_refractionHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void WaterFramebuffers::unbind(int windowWidth, int windowHeight)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);
}
