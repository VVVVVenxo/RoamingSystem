#include "ChunkedTerrain.h"
#include <iostream>
#include <algorithm>

ChunkedTerrain::ChunkedTerrain()
    : m_size(100.0f)
    , m_maxHeight(20.0f)
    , m_chunkSize(64)
    , m_chunksPerRow(0)
    , m_generated(false)
    , m_visibleChunks(0)
    , m_renderedTriangles(0)
    , m_totalVertices(0)
{
}

ChunkedTerrain::~ChunkedTerrain()
{
}

bool ChunkedTerrain::generate(const std::string& heightmapPath, float size, float maxHeight, int chunkSize)
{
    m_size = size;
    m_maxHeight = maxHeight;
    m_chunkSize = chunkSize;
    m_generated = false;
    m_chunks.clear();
    
    if (!m_heightmap.load(heightmapPath))
    {
        std::cerr << "ERROR::CHUNKED_TERRAIN::FAILED_TO_LOAD_HEIGHTMAP" << std::endl;
        return false;
    }
    
    int width = m_heightmap.getWidth();
    int height = m_heightmap.getGridHeight();
    
    m_chunksPerRow = (width - 1) / m_chunkSize;
    if ((width - 1) % m_chunkSize != 0) m_chunksPerRow++;
    
    int chunksPerCol = (height - 1) / m_chunkSize;
    if ((height - 1) % m_chunkSize != 0) chunksPerCol++;
    
    m_chunks.reserve(m_chunksPerRow * chunksPerCol);
    
    for (int cz = 0; cz < chunksPerCol; cz++)
    {
        for (int cx = 0; cx < m_chunksPerRow; cx++)
        {
            int startX = cx * m_chunkSize;
            int startZ = cz * m_chunkSize;
            int actualChunkSizeX = std::min(m_chunkSize, width - 1 - startX);
            int actualChunkSizeZ = std::min(m_chunkSize, height - 1 - startZ);
            int actualChunkSize = std::min(actualChunkSizeX, actualChunkSizeZ);
            
            if (actualChunkSize <= 0) continue;
            
            TerrainChunk chunk;
            chunk.generate(m_heightmap, startX, startZ, actualChunkSize, size, size, maxHeight);
            m_chunks.push_back(std::move(chunk));
        }
    }
    
    // Calculate total vertices (at LOD 0)
    m_totalVertices = 0;
    for (const auto& chunk : m_chunks)
    {
        m_totalVertices += chunk.getTriangleCount(0) * 3;
    }
    
    m_generated = true;
    
    std::cout << "ChunkedTerrain generated: " << m_chunks.size() << " chunks ("
              << m_chunksPerRow << "x" << chunksPerCol << "), chunk size: " << m_chunkSize << std::endl;
    
    return true;
}

void ChunkedTerrain::render(Shader& shader, const glm::vec3& cameraPos, const glm::mat4& viewProjection)
{
    if (!m_generated) return;
    
    if (m_enableFrustumCulling)
    {
        m_frustum.update(viewProjection);
    }
    
    m_visibleChunks = 0;
    m_renderedTriangles = 0;
    
    for (auto& chunk : m_chunks)
    {
        // Frustum culling
        if (m_enableFrustumCulling)
        {
            if (!m_frustum.isBoxVisible(chunk.getMin(), chunk.getMax()))
            {
                continue;
            }
        }
        
        // Calculate LOD based on distance
        int lod = 0;
        if (m_enableLOD)
        {
            float distance = glm::distance(cameraPos, chunk.getCenter());
            lod = calculateLOD(distance);
        }
        
        chunk.render(lod);
        m_visibleChunks++;
        m_renderedTriangles += chunk.getTriangleCount(lod);
    }
}

int ChunkedTerrain::calculateLOD(float distance) const
{
    for (int i = 0; i < 4; i++)
    {
        if (distance < m_lodDistances[i])
        {
            return i;
        }
    }
    return 3;
}

float ChunkedTerrain::getHeightAt(float worldX, float worldZ) const
{
    if (!m_generated) return 0.0f;
    
    float halfSize = m_size * 0.5f;
    float normalizedX = (worldX + halfSize) / m_size;
    float normalizedZ = (worldZ + halfSize) / m_size;
    
    float pixelX = normalizedX * static_cast<float>(m_heightmap.getWidth() - 1);
    float pixelZ = normalizedZ * static_cast<float>(m_heightmap.getGridHeight() - 1);
    
    return m_heightmap.getHeightInterpolated(pixelX, pixelZ) * m_maxHeight;
}
