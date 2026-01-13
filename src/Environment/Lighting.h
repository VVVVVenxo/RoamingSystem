#ifndef LIGHTING_H
#define LIGHTING_H

#include <glm/glm.hpp>

class Lighting
{
public:
    Lighting();
    
    // Update lighting (for auto time advance)
    void update(float deltaTime);
    
    // Time control (0-24 hours)
    float m_timeOfDay;      // Current time (0 = midnight, 12 = noon)
    float m_daySpeed;       // Hours per second when auto-advancing
    bool m_autoAdvance;     // Whether time advances automatically
    
    // Calculated values
    glm::vec3 getSunDirection() const;
    glm::vec3 getSunColor() const;
    glm::vec3 getAmbientColor() const;
    glm::vec3 getSkyColor() const;
    float getSunIntensity() const;
    
private:
    // Helper to get sun height (y component of direction)
    float getSunHeight() const;
};

#endif
