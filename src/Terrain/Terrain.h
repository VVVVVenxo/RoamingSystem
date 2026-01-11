#ifndef TERRAIN_H
#define TERRAIN_H

#include "HeightmapLoader.h"
#include "Core/Mesh.h"
#include "Core/Shader.h"
#include <glm/glm.hpp>
#include <string>

class Terrain
{
public:
    Terrain();
    ~Terrain();

    // Generate terrain from heightmap
    bool generate(const std::string& heightmapPath, float size, float maxHeight);

    // Render terrain
    void render(Shader& shader);

    // Get terrain height at world coordinates (for camera collision)
    float getHeightAt(float worldX, float worldZ) const;

    // Accessors
    float getSize() const { return m_size; }
    float getMaxHeight() const { return m_maxHeight; }
    bool isGenerated() const { return m_generated; }
    int getGridWidth() const { return m_heightmap.getWidth(); }
    int getGridHeight() const { return m_heightmap.getGridHeight(); }

private:
    HeightmapLoader m_heightmap;
    Mesh m_mesh;
    float m_size;
    float m_maxHeight;
    bool m_generated;

    void generateMesh();
    glm::vec3 calculateNormal(int x, int z);
};

#endif
