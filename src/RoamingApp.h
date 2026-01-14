#ifndef ROAMING_APP_H
#define ROAMING_APP_H

#include "Application.h"
#include "Core/Camera.h"
#include "Core/Shader.h"
#include "Core/Texture.h"
#include "Core/Mesh.h"
#include "Terrain/Terrain.h"
#include "Environment/Skybox.h"
#include "Environment/Lighting.h"
#include "Water/Water.h"
#include "Water/WaterFramebuffers.h"
#include "Editor/SceneSettings.h"
#include "PostProcess/SSAO.h"

class RoamingApp : public Application
{
public:
    RoamingApp();
    virtual ~RoamingApp() = default;

protected:
    void onInit() override;
    void onUpdate(float deltaTime) override;
    void onRender() override;
    void onImGui() override;
    void onScroll(float yoffset) override;

private:
    void processInput(float deltaTime);
    void renderScene(const glm::mat4& view, const glm::mat4& projection, const glm::vec4& clipPlane);
    void saveSettings();
    void loadSettings();
    void applySettings(const SceneSettings& settings);
    SceneSettings gatherSettings();

    Camera m_camera;
    
    // Skybox
    Skybox m_skybox;
    
    // Lighting
    Lighting m_lighting;
    
    // Terrain
    Terrain m_terrain;
    Shader m_terrainShader;
    Texture m_grassTexture;
    Texture m_rockTexture;
    Texture m_snowTexture;
    Texture m_grassNormalMap;
    Texture m_rockNormalMap;
    Texture m_snowNormalMap;
    
    // Terrain parameters
    float m_terrainSize;
    float m_terrainMaxHeight;
    float m_textureTiling;
    glm::vec3 m_lightDir;
    float m_grassMaxHeight;
    float m_rockMaxHeight;
    float m_slopeThreshold;
    bool m_useTerrainTextures;
    bool m_useNormalMaps;
    float m_normalMapStrength;
    bool m_wireframeMode;
    
    // Water
    Water m_water;
    WaterFramebuffers m_waterFBOs;
    float m_waterHeight;
    bool m_enableWater;
    
    // Time for animations
    float m_time;
    
    // Camera modes
    bool m_groundWalkMode;
    float m_playerHeight;
    
    // Fog
    bool m_enableFog;
    float m_fogDensity;
    
    // SSAO
    SSAO m_ssao;
    Shader m_gbufferShader;
    bool m_enableSSAO;
    float m_ssaoRadius;
    float m_ssaoBias;
    float m_ssaoIntensity;
    int m_ssaoKernelSize;
    
    // Performance stats
    int m_drawCalls;
    
    // Legacy cube (for reference)
    Shader m_cubeShader;
    Texture m_cubeTexture;
    Mesh m_cubeMesh;
    bool m_showCube;
};

#endif
