#include "SSAO.h"
#include <random>
#include <iostream>

SSAO::SSAO()
    : m_gBufferFBO(0)
    , m_gPosition(0)
    , m_gNormal(0)
    , m_gDepth(0)
    , m_ssaoFBO(0)
    , m_ssaoTexture(0)
    , m_ssaoBlurFBO(0)
    , m_ssaoBlurTexture(0)
    , m_noiseTexture(0)
    , m_quadVAO(0)
    , m_quadVBO(0)
    , m_width(0)
    , m_height(0)
    , m_initialized(false)
    , m_enabled(true)
    , m_radius(0.5f)
    , m_bias(0.025f)
    , m_intensity(1.0f)
    , m_kernelSize(32)
{
}

SSAO::~SSAO()
{
    release();
}

void SSAO::release()
{
    if (m_gBufferFBO) { glDeleteFramebuffers(1, &m_gBufferFBO); m_gBufferFBO = 0; }
    if (m_gPosition) { glDeleteTextures(1, &m_gPosition); m_gPosition = 0; }
    if (m_gNormal) { glDeleteTextures(1, &m_gNormal); m_gNormal = 0; }
    if (m_gDepth) { glDeleteRenderbuffers(1, &m_gDepth); m_gDepth = 0; }
    
    if (m_ssaoFBO) { glDeleteFramebuffers(1, &m_ssaoFBO); m_ssaoFBO = 0; }
    if (m_ssaoTexture) { glDeleteTextures(1, &m_ssaoTexture); m_ssaoTexture = 0; }
    
    if (m_ssaoBlurFBO) { glDeleteFramebuffers(1, &m_ssaoBlurFBO); m_ssaoBlurFBO = 0; }
    if (m_ssaoBlurTexture) { glDeleteTextures(1, &m_ssaoBlurTexture); m_ssaoBlurTexture = 0; }
    
    if (m_noiseTexture) { glDeleteTextures(1, &m_noiseTexture); m_noiseTexture = 0; }
    
    if (m_quadVAO) { glDeleteVertexArrays(1, &m_quadVAO); m_quadVAO = 0; }
    if (m_quadVBO) { glDeleteBuffers(1, &m_quadVBO); m_quadVBO = 0; }
    
    m_initialized = false;
}

float SSAO::lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void SSAO::init(int width, int height)
{
    m_width = width;
    m_height = height;
    
    // Load shaders
    if (!m_ssaoShader.load("shaders/ssao.vert", "shaders/ssao.frag"))
    {
        std::cerr << "ERROR::SSAO::FAILED_TO_LOAD_SSAO_SHADER" << std::endl;
        return;
    }
    
    if (!m_blurShader.load("shaders/ssao.vert", "shaders/ssao_blur.frag"))
    {
        std::cerr << "ERROR::SSAO::FAILED_TO_LOAD_BLUR_SHADER" << std::endl;
        return;
    }
    
    generateKernel();
    generateNoiseTexture();
    createGBuffer();
    createSSAOBuffer();
    createBlurBuffer();
    setupQuad();
    
    m_initialized = true;
    std::cout << "SSAO initialized: " << width << "x" << height << std::endl;
}

void SSAO::resize(int width, int height)
{
    if (width == m_width && height == m_height) return;
    
    m_width = width;
    m_height = height;
    
    // Recreate buffers
    if (m_gBufferFBO) { glDeleteFramebuffers(1, &m_gBufferFBO); m_gBufferFBO = 0; }
    if (m_gPosition) { glDeleteTextures(1, &m_gPosition); m_gPosition = 0; }
    if (m_gNormal) { glDeleteTextures(1, &m_gNormal); m_gNormal = 0; }
    if (m_gDepth) { glDeleteRenderbuffers(1, &m_gDepth); m_gDepth = 0; }
    if (m_ssaoFBO) { glDeleteFramebuffers(1, &m_ssaoFBO); m_ssaoFBO = 0; }
    if (m_ssaoTexture) { glDeleteTextures(1, &m_ssaoTexture); m_ssaoTexture = 0; }
    if (m_ssaoBlurFBO) { glDeleteFramebuffers(1, &m_ssaoBlurFBO); m_ssaoBlurFBO = 0; }
    if (m_ssaoBlurTexture) { glDeleteTextures(1, &m_ssaoBlurTexture); m_ssaoBlurTexture = 0; }
    
    createGBuffer();
    createSSAOBuffer();
    createBlurBuffer();
}

void SSAO::generateKernel()
{
    std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
    std::default_random_engine generator;
    
    m_ssaoKernel.clear();
    for (int i = 0; i < 64; i++)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        
        // Scale samples to be more clustered near origin
        float scale = static_cast<float>(i) / 64.0f;
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        
        m_ssaoKernel.push_back(sample);
    }
}

void SSAO::generateNoiseTexture()
{
    std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
    std::default_random_engine generator;
    
    std::vector<glm::vec3> ssaoNoise;
    for (int i = 0; i < 16; i++)
    {
        glm::vec3 noise(
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator) * 2.0f - 1.0f,
            0.0f
        );
        ssaoNoise.push_back(noise);
    }
    
    glGenTextures(1, &m_noiseTexture);
    glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void SSAO::createGBuffer()
{
    glGenFramebuffers(1, &m_gBufferFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBO);
    
    // Position buffer (view space)
    glGenTextures(1, &m_gPosition);
    glBindTexture(GL_TEXTURE_2D, m_gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0);
    
    // Normal buffer (view space)
    glGenTextures(1, &m_gNormal);
    glBindTexture(GL_TEXTURE_2D, m_gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormal, 0);
    
    // Depth buffer
    glGenRenderbuffers(1, &m_gDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_gDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gDepth);
    
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "ERROR::SSAO::GBUFFER_FRAMEBUFFER_NOT_COMPLETE" << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::createSSAOBuffer()
{
    glGenFramebuffers(1, &m_ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO);
    
    glGenTextures(1, &m_ssaoTexture);
    glBindTexture(GL_TEXTURE_2D, m_ssaoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoTexture, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "ERROR::SSAO::SSAO_FRAMEBUFFER_NOT_COMPLETE" << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::createBlurBuffer()
{
    glGenFramebuffers(1, &m_ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoBlurFBO);
    
    glGenTextures(1, &m_ssaoBlurTexture);
    glBindTexture(GL_TEXTURE_2D, m_ssaoBlurTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoBlurTexture, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "ERROR::SSAO::BLUR_FRAMEBUFFER_NOT_COMPLETE" << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::setupQuad()
{
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
}

void SSAO::bindGBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBO);
    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SSAO::renderSSAO(const glm::mat4& projection, const glm::mat4& view)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    
    m_ssaoShader.use();
    
    // Send kernel samples
    for (int i = 0; i < 64; i++)
    {
        m_ssaoShader.setVec3("uSamples[" + std::to_string(i) + "]", m_ssaoKernel[i]);
    }
    
    m_ssaoShader.setMat4("uProjection", projection);
    m_ssaoShader.setFloat("uRadius", m_radius);
    m_ssaoShader.setFloat("uBias", m_bias);
    m_ssaoShader.setInt("uKernelSize", m_kernelSize);
    m_ssaoShader.setVec2("uNoiseScale", glm::vec2(m_width / 4.0f, m_height / 4.0f));
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gPosition);
    m_ssaoShader.setInt("uPositionTex", 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gNormal);
    m_ssaoShader.setInt("uNormalTex", 1);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
    m_ssaoShader.setInt("uNoiseTex", 2);
    
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void SSAO::renderBlur()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    
    m_blurShader.use();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ssaoTexture);
    m_blurShader.setInt("uSSAOInput", 0);
    
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void SSAO::unbind(int windowWidth, int windowHeight)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);
}
