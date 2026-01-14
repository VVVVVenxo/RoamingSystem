/**
 * @file Frustum.h
 * @brief View frustum culling class
 * @author LuNingfang
 */

#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <glm/glm.hpp>

class Frustum
{
public:
    Frustum();
    
    void update(const glm::mat4& viewProjection);
    bool isBoxVisible(const glm::vec3& min, const glm::vec3& max) const;
    
private:
    enum Planes { LEFT = 0, RIGHT, BOTTOM, TOP, NEAR, FAR, COUNT };
    glm::vec4 m_planes[COUNT];
    
    void normalizePlane(glm::vec4& plane);
};

#endif
