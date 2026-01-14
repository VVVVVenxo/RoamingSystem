#ifndef TERRAIN_CHUNK_H
#define TERRAIN_CHUNK_H

#include "HeightmapLoader.h"
#include "Core/Mesh.h"
#include <glm/glm.hpp>

class TerrainChunk
{
public:
    static const int LOD_LEVELS = 4;
    
    TerrainChunk();
    ~TerrainChunk();
    
    TerrainChunk(TerrainChunk&& other) noexcept;
    TerrainChunk& operator=(TerrainChunk&& other) noexcept;
    
    TerrainChunk(const TerrainChunk&) = delete;
    TerrainChunk& operator=(const TerrainChunk&) = delete;
    
    void generate(const HeightmapLoader& heightmap,
                  int startX, int startZ, int chunkSize,
                  float worldSize, float terrainSize, float maxHeight);
    
    void render(int lodLevel);
    
    glm::vec3 getMin() const { return m_min; }
    glm::vec3 getMax() const { return m_max; }
    glm::vec3 getCenter() const { return m_center; }
    
    int getTriangleCount(int lodLevel) const;
    bool isGenerated() const { return m_generated; }
    
private:
    Mesh m_lodMeshes[LOD_LEVELS];
    int m_triangleCounts[LOD_LEVELS];
    glm::vec3 m_min, m_max, m_center;
    bool m_generated;
    
    void generateLODMesh(const HeightmapLoader& heightmap,
                         int startX, int startZ, int chunkSize,
                         float worldOffsetX, float worldOffsetZ,
                         float cellSize, float maxHeight, int lodLevel);
    
    glm::vec3 calculateNormal(const HeightmapLoader& heightmap,
                              int x, int z, float cellSize, float maxHeight);
    glm::vec3 calculateTangent(const glm::vec3& normal);
};

#endif
