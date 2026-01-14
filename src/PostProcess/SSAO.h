#ifndef SSAO_H
#define SSAO_H

#include "Core/Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class SSAO
{
public:
    SSAO();
    ~SSAO();

    void init(int width, int height);
    void resize(int width, int height);
    
    // Render passes
    void bindGBuffer();
    void renderSSAO(const glm::mat4& projection, const glm::mat4& view);
    void renderBlur();
    void unbind(int windowWidth, int windowHeight);
    
    // Get final SSAO texture
    unsigned int getSSAOTexture() const { return m_ssaoBlurTexture; }
    unsigned int getPositionTexture() const { return m_gPosition; }
    unsigned int getNormalTexture() const { return m_gNormal; }
    
    bool isInitialized() const { return m_initialized; }
    
    // Public parameters
    bool m_enabled;
    float m_radius;
    float m_bias;
    float m_intensity;
    int m_kernelSize;

private:
    // G-Buffer
    unsigned int m_gBufferFBO;
    unsigned int m_gPosition;
    unsigned int m_gNormal;
    unsigned int m_gDepth;
    
    // SSAO
    unsigned int m_ssaoFBO;
    unsigned int m_ssaoTexture;
    
    // Blur
    unsigned int m_ssaoBlurFBO;
    unsigned int m_ssaoBlurTexture;
    
    // Noise texture and kernel
    unsigned int m_noiseTexture;
    std::vector<glm::vec3> m_ssaoKernel;
    
    // Screen quad
    unsigned int m_quadVAO;
    unsigned int m_quadVBO;
    
    // Shaders
    Shader m_ssaoShader;
    Shader m_blurShader;
    
    int m_width;
    int m_height;
    bool m_initialized;
    
    void generateKernel();
    void generateNoiseTexture();
    void createGBuffer();
    void createSSAOBuffer();
    void createBlurBuffer();
    void setupQuad();
    void release();
    
    float lerp(float a, float b, float f);
};

#endif
