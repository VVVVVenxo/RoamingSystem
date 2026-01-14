#include "SceneSettings.h"
#include <fstream>
#include <iostream>
#include <sstream>

std::string SceneSettingsManager::getDefaultPath()
{
    return "settings.ini";
}

bool SceneSettingsManager::save(const SceneSettings& settings, const std::string& filepath)
{
    std::ofstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "ERROR::SCENE_SETTINGS::FAILED_TO_OPEN_FILE_FOR_WRITING: " << filepath << std::endl;
        return false;
    }

    file << "[Terrain]" << std::endl;
    file << "textureTiling=" << settings.textureTiling << std::endl;
    file << "grassMaxHeight=" << settings.grassMaxHeight << std::endl;
    file << "rockMaxHeight=" << settings.rockMaxHeight << std::endl;
    file << "slopeThreshold=" << settings.slopeThreshold << std::endl;
    file << "useTerrainTextures=" << (settings.useTerrainTextures ? 1 : 0) << std::endl;

    file << std::endl << "[Water]" << std::endl;
    file << "waterHeight=" << settings.waterHeight << std::endl;
    file << "waveSpeed=" << settings.waveSpeed << std::endl;
    file << "waveStrength=" << settings.waveStrength << std::endl;
    file << "waterTiling=" << settings.waterTiling << std::endl;
    file << "shineDamper=" << settings.shineDamper << std::endl;
    file << "reflectivity=" << settings.reflectivity << std::endl;
    file << "waterColorR=" << settings.waterColor.r << std::endl;
    file << "waterColorG=" << settings.waterColor.g << std::endl;
    file << "waterColorB=" << settings.waterColor.b << std::endl;
    file << "enableWater=" << (settings.enableWater ? 1 : 0) << std::endl;

    file << std::endl << "[Lighting]" << std::endl;
    file << "timeOfDay=" << settings.timeOfDay << std::endl;
    file << "daySpeed=" << settings.daySpeed << std::endl;
    file << "autoAdvance=" << (settings.autoAdvance ? 1 : 0) << std::endl;

    file << std::endl << "[Fog]" << std::endl;
    file << "enableFog=" << (settings.enableFog ? 1 : 0) << std::endl;
    file << "fogDensity=" << settings.fogDensity << std::endl;

    file << std::endl << "[SSAO]" << std::endl;
    file << "enableSSAO=" << (settings.enableSSAO ? 1 : 0) << std::endl;
    file << "ssaoRadius=" << settings.ssaoRadius << std::endl;
    file << "ssaoBias=" << settings.ssaoBias << std::endl;
    file << "ssaoIntensity=" << settings.ssaoIntensity << std::endl;
    file << "ssaoKernelSize=" << settings.ssaoKernelSize << std::endl;

    file << std::endl << "[Camera]" << std::endl;
    file << "cameraPosX=" << settings.cameraPos.x << std::endl;
    file << "cameraPosY=" << settings.cameraPos.y << std::endl;
    file << "cameraPosZ=" << settings.cameraPos.z << std::endl;
    file << "cameraYaw=" << settings.cameraYaw << std::endl;
    file << "cameraPitch=" << settings.cameraPitch << std::endl;
    file << "moveSpeed=" << settings.moveSpeed << std::endl;
    file << "mouseSensitivity=" << settings.mouseSensitivity << std::endl;
    file << "groundWalkMode=" << (settings.groundWalkMode ? 1 : 0) << std::endl;
    file << "playerHeight=" << settings.playerHeight << std::endl;

    file << std::endl << "[Display]" << std::endl;
    file << "wireframeMode=" << (settings.wireframeMode ? 1 : 0) << std::endl;
    file << "showCube=" << (settings.showCube ? 1 : 0) << std::endl;

    file.close();
    std::cout << "Scene settings saved to: " << filepath << std::endl;
    return true;
}

bool SceneSettingsManager::load(SceneSettings& settings, const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cout << "Settings file not found: " << filepath << " (using defaults)" << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // Skip empty lines and section headers
        if (line.empty() || line[0] == '[')
        {
            continue;
        }

        // Parse key=value
        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos)
        {
            continue;
        }

        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);

        // Terrain
        if (key == "textureTiling") settings.textureTiling = std::stof(value);
        else if (key == "grassMaxHeight") settings.grassMaxHeight = std::stof(value);
        else if (key == "rockMaxHeight") settings.rockMaxHeight = std::stof(value);
        else if (key == "slopeThreshold") settings.slopeThreshold = std::stof(value);
        else if (key == "useTerrainTextures") settings.useTerrainTextures = (std::stoi(value) != 0);
        
        // Water
        else if (key == "waterHeight") settings.waterHeight = std::stof(value);
        else if (key == "waveSpeed") settings.waveSpeed = std::stof(value);
        else if (key == "waveStrength") settings.waveStrength = std::stof(value);
        else if (key == "waterTiling") settings.waterTiling = std::stof(value);
        else if (key == "shineDamper") settings.shineDamper = std::stof(value);
        else if (key == "reflectivity") settings.reflectivity = std::stof(value);
        else if (key == "waterColorR") settings.waterColor.r = std::stof(value);
        else if (key == "waterColorG") settings.waterColor.g = std::stof(value);
        else if (key == "waterColorB") settings.waterColor.b = std::stof(value);
        else if (key == "enableWater") settings.enableWater = (std::stoi(value) != 0);
        
        // Lighting
        else if (key == "timeOfDay") settings.timeOfDay = std::stof(value);
        else if (key == "daySpeed") settings.daySpeed = std::stof(value);
        else if (key == "autoAdvance") settings.autoAdvance = (std::stoi(value) != 0);
        
        // Fog
        else if (key == "enableFog") settings.enableFog = (std::stoi(value) != 0);
        else if (key == "fogDensity") settings.fogDensity = std::stof(value);
        
        // SSAO
        else if (key == "enableSSAO") settings.enableSSAO = (std::stoi(value) != 0);
        else if (key == "ssaoRadius") settings.ssaoRadius = std::stof(value);
        else if (key == "ssaoBias") settings.ssaoBias = std::stof(value);
        else if (key == "ssaoIntensity") settings.ssaoIntensity = std::stof(value);
        else if (key == "ssaoKernelSize") settings.ssaoKernelSize = std::stoi(value);
        
        // Camera
        else if (key == "cameraPosX") settings.cameraPos.x = std::stof(value);
        else if (key == "cameraPosY") settings.cameraPos.y = std::stof(value);
        else if (key == "cameraPosZ") settings.cameraPos.z = std::stof(value);
        else if (key == "cameraYaw") settings.cameraYaw = std::stof(value);
        else if (key == "cameraPitch") settings.cameraPitch = std::stof(value);
        else if (key == "moveSpeed") settings.moveSpeed = std::stof(value);
        else if (key == "mouseSensitivity") settings.mouseSensitivity = std::stof(value);
        else if (key == "groundWalkMode") settings.groundWalkMode = (std::stoi(value) != 0);
        else if (key == "playerHeight") settings.playerHeight = std::stof(value);
        
        // Display
        else if (key == "wireframeMode") settings.wireframeMode = (std::stoi(value) != 0);
        else if (key == "showCube") settings.showCube = (std::stoi(value) != 0);
    }

    file.close();
    std::cout << "Scene settings loaded from: " << filepath << std::endl;
    return true;
}
