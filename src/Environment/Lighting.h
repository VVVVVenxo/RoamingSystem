/**
 * @file Lighting.h
 * @brief Day/night cycle lighting system
 * @author LuNingfang
 */

#ifndef LIGHTING_H
#define LIGHTING_H

#include <glm/glm.hpp>

class Lighting
{
public:
    Lighting();
    
    /**
     * @brief Update time of day (for auto-advance)
     * @param deltaTime Frame delta time in seconds
     */
    void update(float deltaTime);
    
    // Time control (0-24 hours)
    float m_timeOfDay;      // Current time (0 = midnight, 12 = noon)
    float m_daySpeed;       // Hours per second when auto-advancing
    bool m_autoAdvance;     // Whether time advances automatically
    
    /**
     * @brief Get sun direction vector (pointing toward sun)
     * @return Normalized direction vector
     */
    glm::vec3 getSunDirection() const;
    
    /**
     * @brief Get sun light color (warm at sunset, white at noon)
     */
    glm::vec3 getSunColor() const;
    
    /**
     * @brief Get ambient light color
     */
    glm::vec3 getAmbientColor() const;
    
    /**
     * @brief Get sky clear color (for glClearColor)
     */
    glm::vec3 getSkyColor() const;
    
    /**
     * @brief Get fog color (follows sky color but desaturated)
     */
    glm::vec3 getFogColor() const;
    
    /**
     * @brief Get sun intensity (1.0 at noon, dims toward horizon)
     */
    float getSunIntensity() const;
    
private:
    float getSunHeight() const;
};

#endif
