/**
 * @file ChunkedTerrain.h
 * @brief Chunked terrain manager with LOD and frustum culling
 * @author LuNingfang
 */

#ifndef CHUNKED_TERRAIN_H
#define CHUNKED_TERRAIN_H

#include "HeightmapLoader.h"
#include "TerrainChunk.h"
#include "Frustum.h"
#include "Core/Shader.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>

class ChunkedTerrain
{
public:
    ChunkedTerrain();
    ~ChunkedTerrain();
    
    bool generate(const std::string& heightmapPath, float size, float maxHeight, int chunkSize = 64);
    
    void render(Shader& shader, const glm::vec3& cameraPos, const glm::mat4& viewProjection);
    
    float getHeightAt(float worldX, float worldZ) const;
    
    float getSize() const { return m_size; }
    float getMaxHeight() const { return m_maxHeight; }
    bool isGenerated() const { return m_generated; }
    int getGridWidth() const { return m_heightmap.getWidth(); }
    int getGridHeight() const { return m_heightmap.getGridHeight(); }
    
    int getTotalChunks() const { return static_cast<int>(m_chunks.size()); }
    int getVisibleChunks() const { return m_visibleChunks; }
    int getCulledChunks() const { return getTotalChunks() - m_visibleChunks; }
    int getRenderedTriangles() const { return m_renderedTriangles; }
    int getTotalVertices() const { return m_totalVertices; }
    
    float m_lodDistances[4] = { 100.0f, 200.0f, 400.0f, 800.0f };
    bool m_enableFrustumCulling = true;
    bool m_enableLOD = true;
    
private:
    HeightmapLoader m_heightmap;
    std::vector<TerrainChunk> m_chunks;
    Frustum m_frustum;
    
    float m_size;
    float m_maxHeight;
    int m_chunkSize;
    int m_chunksPerRow;
    bool m_generated;
    
    int m_visibleChunks;
    int m_renderedTriangles;
    int m_totalVertices;
    
    int calculateLOD(float distance) const;
};

#endif
