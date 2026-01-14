/**
 * @file SSAO.h
 * @brief Screen Space Ambient Occlusion post-processing effect
 * @author LuNingfang
 */

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

    /**
     * @brief Initialize SSAO system
     * @param width Screen width
     * @param height Screen height
     */
    void init(int width, int height);
    
    /**
     * @brief Resize buffers (when window resizes)
     */
    void resize(int width, int height);
    
    /**
     * @brief Bind G-Buffer for geometry pass
     */
    void bindGBuffer();
    
    /**
     * @brief Calculate SSAO occlusion factors
     * @param projection Projection matrix
     * @param view View matrix
     */
    void renderSSAO(const glm::mat4& projection, const glm::mat4& view);
    
    /**
     * @brief Blur SSAO to remove noise
     */
    void renderBlur();
    
    /**
     * @brief Unbind FBO and restore default framebuffer
     */
    void unbind(int windowWidth, int windowHeight);
    
    // Texture getters
    unsigned int getSSAOTexture() const { return m_ssaoBlurTexture; }
    unsigned int getPositionTexture() const { return m_gPosition; }
    unsigned int getNormalTexture() const { return m_gNormal; }
    
    bool isInitialized() const { return m_initialized; }
    
    // Public parameters for ImGui
    bool m_enabled;
    float m_radius;         // Sampling radius
    float m_bias;           // Depth bias to avoid acne
    float m_intensity;      // Occlusion intensity
    int m_kernelSize;       // Number of samples

private:
    // G-Buffer (stores position and normal in view space)
    unsigned int m_gBufferFBO;
    unsigned int m_gPosition;
    unsigned int m_gNormal;
    unsigned int m_gDepth;
    
    // SSAO calculation
    unsigned int m_ssaoFBO;
    unsigned int m_ssaoTexture;
    
    // SSAO blur
    unsigned int m_ssaoBlurFBO;
    unsigned int m_ssaoBlurTexture;
    
    // Random samples and noise
    unsigned int m_noiseTexture;
    std::vector<glm::vec3> m_ssaoKernel;
    
    // Full-screen quad
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
