#ifndef TERRAIN_H
#define TERRAIN_H

#include "ChunkedTerrain.h"
#include "Core/Shader.h"
#include <glm/glm.hpp>
#include <string>

class Terrain
{
public:
    Terrain();
    ~Terrain();

    bool generate(const std::string& heightmapPath, float size, float maxHeight);

    void render(Shader& shader, const glm::vec3& cameraPos, const glm::mat4& viewProjection);

    float getHeightAt(float worldX, float worldZ) const;

    // Accessors
    float getSize() const { return m_chunkedTerrain.getSize(); }
    float getMaxHeight() const { return m_chunkedTerrain.getMaxHeight(); }
    bool isGenerated() const { return m_chunkedTerrain.isGenerated(); }
    int getGridWidth() const { return m_chunkedTerrain.getGridWidth(); }
    int getGridHeight() const { return m_chunkedTerrain.getGridHeight(); }
    
    // Statistics
    int getVertexCount() const { return m_chunkedTerrain.getTotalVertices(); }
    int getTriangleCount() const { return m_chunkedTerrain.getRenderedTriangles(); }
    int getTotalChunks() const { return m_chunkedTerrain.getTotalChunks(); }
    int getVisibleChunks() const { return m_chunkedTerrain.getVisibleChunks(); }
    int getCulledChunks() const { return m_chunkedTerrain.getCulledChunks(); }
    
    // LOD/Culling settings (exposed for ImGui)
    ChunkedTerrain& getChunkedTerrain() { return m_chunkedTerrain; }

private:
    ChunkedTerrain m_chunkedTerrain;
};

#endif
