#ifndef ROAMING_APP_H
#define ROAMING_APP_H

#include "Application.h"
#include "Core/Camera.h"
#include "Core/Shader.h"
#include "Core/Texture.h"
#include "Core/Mesh.h"
#include "Terrain/Terrain.h"

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

    Camera m_camera;
    
    // Terrain
    Terrain m_terrain;
    Shader m_terrainShader;
    Texture m_grassTexture;
    Texture m_rockTexture;
    Texture m_snowTexture;
    
    // Terrain parameters
    float m_terrainSize;
    float m_terrainMaxHeight;
    float m_textureTiling;
    glm::vec3 m_lightDir;
    float m_grassMaxHeight;
    float m_rockMaxHeight;
    float m_slopeThreshold;
    bool m_useTerrainTextures;
    bool m_wireframeMode;
    
    // Legacy cube (for reference)
    Shader m_cubeShader;
    Texture m_cubeTexture;
    Mesh m_cubeMesh;
    bool m_showCube;
};

#endif
