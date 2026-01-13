#include "Terrain.h"
#include <iostream>

Terrain::Terrain()
{
}

Terrain::~Terrain()
{
}

bool Terrain::generate(const std::string& heightmapPath, float size, float maxHeight)
{
    return m_chunkedTerrain.generate(heightmapPath, size, maxHeight, 64);
}

void Terrain::render(Shader& shader, const glm::vec3& cameraPos, const glm::mat4& viewProjection)
{
    m_chunkedTerrain.render(shader, cameraPos, viewProjection);
}

float Terrain::getHeightAt(float worldX, float worldZ) const
{
    return m_chunkedTerrain.getHeightAt(worldX, worldZ);
}
