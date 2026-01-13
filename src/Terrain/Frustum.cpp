#include "Frustum.h"

Frustum::Frustum()
{
    for (int i = 0; i < COUNT; i++)
    {
        m_planes[i] = glm::vec4(0.0f);
    }
}

void Frustum::update(const glm::mat4& vp)
{
    // Extract frustum planes from ViewProjection matrix
    // Left plane
    m_planes[LEFT].x = vp[0][3] + vp[0][0];
    m_planes[LEFT].y = vp[1][3] + vp[1][0];
    m_planes[LEFT].z = vp[2][3] + vp[2][0];
    m_planes[LEFT].w = vp[3][3] + vp[3][0];
    
    // Right plane
    m_planes[RIGHT].x = vp[0][3] - vp[0][0];
    m_planes[RIGHT].y = vp[1][3] - vp[1][0];
    m_planes[RIGHT].z = vp[2][3] - vp[2][0];
    m_planes[RIGHT].w = vp[3][3] - vp[3][0];
    
    // Bottom plane
    m_planes[BOTTOM].x = vp[0][3] + vp[0][1];
    m_planes[BOTTOM].y = vp[1][3] + vp[1][1];
    m_planes[BOTTOM].z = vp[2][3] + vp[2][1];
    m_planes[BOTTOM].w = vp[3][3] + vp[3][1];
    
    // Top plane
    m_planes[TOP].x = vp[0][3] - vp[0][1];
    m_planes[TOP].y = vp[1][3] - vp[1][1];
    m_planes[TOP].z = vp[2][3] - vp[2][1];
    m_planes[TOP].w = vp[3][3] - vp[3][1];
    
    // Near plane
    m_planes[NEAR].x = vp[0][3] + vp[0][2];
    m_planes[NEAR].y = vp[1][3] + vp[1][2];
    m_planes[NEAR].z = vp[2][3] + vp[2][2];
    m_planes[NEAR].w = vp[3][3] + vp[3][2];
    
    // Far plane
    m_planes[FAR].x = vp[0][3] - vp[0][2];
    m_planes[FAR].y = vp[1][3] - vp[1][2];
    m_planes[FAR].z = vp[2][3] - vp[2][2];
    m_planes[FAR].w = vp[3][3] - vp[3][2];
    
    for (int i = 0; i < COUNT; i++)
    {
        normalizePlane(m_planes[i]);
    }
}

void Frustum::normalizePlane(glm::vec4& plane)
{
    float length = glm::length(glm::vec3(plane));
    if (length > 0.0f)
    {
        plane /= length;
    }
}

bool Frustum::isBoxVisible(const glm::vec3& min, const glm::vec3& max) const
{
    for (int i = 0; i < COUNT; i++)
    {
        const glm::vec4& plane = m_planes[i];
        
        // Find the positive vertex (p-vertex)
        glm::vec3 pVertex;
        pVertex.x = (plane.x >= 0.0f) ? max.x : min.x;
        pVertex.y = (plane.y >= 0.0f) ? max.y : min.y;
        pVertex.z = (plane.z >= 0.0f) ? max.z : min.z;
        
        // If p-vertex is outside, the box is completely outside
        float distance = glm::dot(glm::vec3(plane), pVertex) + plane.w;
        if (distance < 0.0f)
        {
            return false;
        }
    }
    return true;
}
