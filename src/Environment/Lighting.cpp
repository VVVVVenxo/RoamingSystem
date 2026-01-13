#include "Lighting.h"
#include <glm/gtc/constants.hpp>
#include <algorithm>

Lighting::Lighting()
    : m_timeOfDay(12.0f)
    , m_daySpeed(0.1f)
    , m_autoAdvance(false)
{
}

void Lighting::update(float deltaTime)
{
    if (m_autoAdvance)
    {
        m_timeOfDay += m_daySpeed * deltaTime;
        
        // Wrap around 24 hours
        while (m_timeOfDay >= 24.0f)
        {
            m_timeOfDay -= 24.0f;
        }
        while (m_timeOfDay < 0.0f)
        {
            m_timeOfDay += 24.0f;
        }
    }
}

glm::vec3 Lighting::getSunDirection() const
{
    // Sun rises at 6:00, peaks at 12:00, sets at 18:00
    // Convert time to angle: 6:00 = 0, 12:00 = PI/2, 18:00 = PI
    float angle = (m_timeOfDay - 6.0f) / 12.0f * glm::pi<float>();
    
    // Sun moves in an arc from east to west
    // x: east-west direction (cos gives east at 6:00, west at 18:00)
    // y: height (sin gives 0 at 6:00/18:00, 1 at 12:00)
    // z: slight offset for visual interest
    return glm::normalize(glm::vec3(
        cos(angle),
        sin(angle),
        -0.3f
    ));
}

float Lighting::getSunHeight() const
{
    return getSunDirection().y;
}

glm::vec3 Lighting::getSunColor() const
{
    float sunHeight = getSunHeight();
    
    // Bright white/yellow at noon, warm orange at sunrise/sunset
    glm::vec3 noonColor = glm::vec3(1.0f, 1.0f, 0.95f);      // Slightly warm white
    glm::vec3 sunsetColor = glm::vec3(1.0f, 0.6f, 0.3f);     // Warm orange
    glm::vec3 nightColor = glm::vec3(0.2f, 0.2f, 0.4f);      // Cool blue moonlight
    
    if (sunHeight > 0.3f)
    {
        // High sun - noon colors
        return noonColor;
    }
    else if (sunHeight > 0.0f)
    {
        // Sunrise/sunset transition
        float t = sunHeight / 0.3f;
        return glm::mix(sunsetColor, noonColor, t);
    }
    else if (sunHeight > -0.3f)
    {
        // Twilight
        float t = (sunHeight + 0.3f) / 0.3f;
        return glm::mix(nightColor, sunsetColor, t);
    }
    else
    {
        // Night
        return nightColor;
    }
}

glm::vec3 Lighting::getAmbientColor() const
{
    float sunHeight = getSunHeight();
    
    // Ambient light color follows sky color but dimmer
    glm::vec3 dayAmbient = glm::vec3(0.4f, 0.45f, 0.5f);
    glm::vec3 sunsetAmbient = glm::vec3(0.3f, 0.25f, 0.2f);
    glm::vec3 nightAmbient = glm::vec3(0.05f, 0.05f, 0.1f);
    
    if (sunHeight > 0.2f)
    {
        return dayAmbient;
    }
    else if (sunHeight > 0.0f)
    {
        float t = sunHeight / 0.2f;
        return glm::mix(sunsetAmbient, dayAmbient, t);
    }
    else if (sunHeight > -0.2f)
    {
        float t = (sunHeight + 0.2f) / 0.2f;
        return glm::mix(nightAmbient, sunsetAmbient, t);
    }
    else
    {
        return nightAmbient;
    }
}

glm::vec3 Lighting::getSkyColor() const
{
    float sunHeight = getSunHeight();
    
    // Sky colors for different times of day
    glm::vec3 dayColor = glm::vec3(0.5f, 0.7f, 1.0f);        // Clear blue sky
    glm::vec3 sunsetColor = glm::vec3(1.0f, 0.5f, 0.2f);     // Orange sunset
    glm::vec3 twilightColor = glm::vec3(0.3f, 0.2f, 0.4f);   // Purple twilight
    glm::vec3 nightColor = glm::vec3(0.02f, 0.02f, 0.08f);   // Dark blue night
    
    if (sunHeight > 0.3f)
    {
        // Clear day
        return dayColor;
    }
    else if (sunHeight > 0.0f)
    {
        // Sunset/sunrise gradient
        float t = sunHeight / 0.3f;
        return glm::mix(sunsetColor, dayColor, t);
    }
    else if (sunHeight > -0.2f)
    {
        // Twilight
        float t = (sunHeight + 0.2f) / 0.2f;
        return glm::mix(twilightColor, sunsetColor, t);
    }
    else if (sunHeight > -0.5f)
    {
        // Deep twilight to night
        float t = (sunHeight + 0.5f) / 0.3f;
        return glm::mix(nightColor, twilightColor, t);
    }
    else
    {
        // Night
        return nightColor;
    }
}

float Lighting::getSunIntensity() const
{
    float sunHeight = getSunHeight();
    
    // Full intensity at noon, diminishes toward horizon
    if (sunHeight > 0.2f)
    {
        return 1.0f;
    }
    else if (sunHeight > 0.0f)
    {
        // Fade toward horizon
        return 0.5f + 0.5f * (sunHeight / 0.2f);
    }
    else if (sunHeight > -0.2f)
    {
        // Twilight - very dim
        return 0.1f + 0.4f * ((sunHeight + 0.2f) / 0.2f);
    }
    else
    {
        // Night - minimal "moonlight"
        return 0.1f;
    }
}
