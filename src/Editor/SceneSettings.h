#ifndef SCENE_SETTINGS_H
#define SCENE_SETTINGS_H

#include <glm/glm.hpp>
#include <string>

// Scene settings structure
struct SceneSettings
{
    // Terrain
    float textureTiling = 32.0f;
    float grassMaxHeight = 0.35f;
    float rockMaxHeight = 0.7f;
    float slopeThreshold = 0.4f;
    bool useTerrainTextures = false;
    
    // Water
    float waterHeight = 10.0f;
    float waveSpeed = 0.03f;
    float waveStrength = 0.02f;
    float waterTiling = 4.0f;
    float shineDamper = 20.0f;
    float reflectivity = 0.6f;
    glm::vec3 waterColor = glm::vec3(0.0f, 0.3f, 0.5f);
    bool enableWater = true;
    
    // Lighting / Day-Night Cycle
    float timeOfDay = 12.0f;
    float daySpeed = 0.1f;
    bool autoAdvance = false;
    
    // Camera
    glm::vec3 cameraPos = glm::vec3(0.0f, 30.0f, 50.0f);
    float cameraYaw = -90.0f;
    float cameraPitch = 0.0f;
    float moveSpeed = 2.5f;
    float mouseSensitivity = 0.1f;
    bool groundWalkMode = false;
    float playerHeight = 1.8f;
    
    // Display
    bool wireframeMode = false;
    bool showCube = false;
};

// Scene settings manager
class SceneSettingsManager
{
public:
    // Save settings to file
    static bool save(const SceneSettings& settings, const std::string& filepath);
    
    // Load settings from file
    static bool load(SceneSettings& settings, const std::string& filepath);
    
    // Get default settings file path
    static std::string getDefaultPath();
};

#endif
