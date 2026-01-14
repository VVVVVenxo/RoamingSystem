#include "TerrainChunk.h"
#include <vector>
#include <algorithm>

namespace {
    template<typename T>
    T clampValue(T val, T minVal, T maxVal) {
        return std::max(minVal, std::min(val, maxVal));
    }
}

TerrainChunk::TerrainChunk()
    : m_min(0.0f)
    , m_max(0.0f)
    , m_center(0.0f)
    , m_generated(false)
{
    for (int i = 0; i < LOD_LEVELS; i++)
    {
        m_triangleCounts[i] = 0;
    }
}

TerrainChunk::~TerrainChunk()
{
}

TerrainChunk::TerrainChunk(TerrainChunk&& other) noexcept
    : m_min(other.m_min)
    , m_max(other.m_max)
    , m_center(other.m_center)
    , m_generated(other.m_generated)
{
    for (int i = 0; i < LOD_LEVELS; i++)
    {
        m_lodMeshes[i] = std::move(other.m_lodMeshes[i]);
        m_triangleCounts[i] = other.m_triangleCounts[i];
    }
    other.m_generated = false;
}

TerrainChunk& TerrainChunk::operator=(TerrainChunk&& other) noexcept
{
    if (this != &other)
    {
        m_min = other.m_min;
        m_max = other.m_max;
        m_center = other.m_center;
        m_generated = other.m_generated;
        
        for (int i = 0; i < LOD_LEVELS; i++)
        {
            m_lodMeshes[i] = std::move(other.m_lodMeshes[i]);
            m_triangleCounts[i] = other.m_triangleCounts[i];
        }
        other.m_generated = false;
    }
    return *this;
}

void TerrainChunk::generate(const HeightmapLoader& heightmap,
                            int startX, int startZ, int chunkSize,
                            float worldSize, float terrainSize, float maxHeight)
{
    int hmWidth = heightmap.getWidth();
    int hmHeight = heightmap.getGridHeight();
    
    float cellSize = terrainSize / static_cast<float>(hmWidth - 1);
    float halfTerrain = terrainSize * 0.5f;
    
    float worldOffsetX = startX * cellSize - halfTerrain;
    float worldOffsetZ = startZ * cellSize - halfTerrain;
    
    // Calculate AABB
    float minY = maxHeight, maxY = 0.0f;
    for (int z = startZ; z <= startZ + chunkSize && z < hmHeight; z++)
    {
        for (int x = startX; x <= startX + chunkSize && x < hmWidth; x++)
        {
            float h = heightmap.getHeight(x, z) * maxHeight;
            minY = std::min(minY, h);
            maxY = std::max(maxY, h);
        }
    }
    
    m_min = glm::vec3(worldOffsetX, minY, worldOffsetZ);
    m_max = glm::vec3(worldOffsetX + chunkSize * cellSize, maxY, worldOffsetZ + chunkSize * cellSize);
    m_center = (m_min + m_max) * 0.5f;
    
    // Generate LOD meshes
    for (int lod = 0; lod < LOD_LEVELS; lod++)
    {
        generateLODMesh(heightmap, startX, startZ, chunkSize,
                        worldOffsetX, worldOffsetZ, cellSize, maxHeight, lod);
    }
    
    m_generated = true;
}

void TerrainChunk::generateLODMesh(const HeightmapLoader& heightmap,
                                   int startX, int startZ, int chunkSize,
                                   float worldOffsetX, float worldOffsetZ,
                                   float cellSize, float maxHeight, int lodLevel)
{
    int step = 1 << lodLevel; // LOD0=1, LOD1=2, LOD2=4, LOD3=8
    int hmWidth = heightmap.getWidth();
    int hmHeight = heightmap.getGridHeight();
    
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    // Generate vertices
    int vertexCountX = 0, vertexCountZ = 0;
    for (int z = startZ; z <= startZ + chunkSize && z < hmHeight; z += step)
    {
        vertexCountX = 0;
        for (int x = startX; x <= startX + chunkSize && x < hmWidth; x += step)
        {
            float worldX = worldOffsetX + (x - startX) * cellSize;
            float worldZ = worldOffsetZ + (z - startZ) * cellSize;
            float worldY = heightmap.getHeight(x, z) * maxHeight;
            
            glm::vec3 normal = calculateNormal(heightmap, x, z, cellSize, maxHeight);
            
            float u = static_cast<float>(x) / static_cast<float>(hmWidth - 1);
            float v = static_cast<float>(z) / static_cast<float>(hmHeight - 1);
            
            glm::vec3 tangent = calculateTangent(normal);
            
            vertices.push_back(worldX);
            vertices.push_back(worldY);
            vertices.push_back(worldZ);
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
            vertices.push_back(u);
            vertices.push_back(v);
            vertices.push_back(tangent.x);
            vertices.push_back(tangent.y);
            vertices.push_back(tangent.z);
            
            vertexCountX++;
        }
        vertexCountZ++;
    }
    
    // Generate indices
    for (int z = 0; z < vertexCountZ - 1; z++)
    {
        for (int x = 0; x < vertexCountX - 1; x++)
        {
            unsigned int topLeft = z * vertexCountX + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * vertexCountX + x;
            unsigned int bottomRight = bottomLeft + 1;
            
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    
    if (!vertices.empty() && !indices.empty())
    {
        m_lodMeshes[lodLevel].setVertices(vertices.data(), vertices.size() * sizeof(float),
                                          VertexLayout::positionNormalTextureTangent());
        m_lodMeshes[lodLevel].setIndices(indices.data(), indices.size());
        m_triangleCounts[lodLevel] = static_cast<int>(indices.size() / 3);
    }
}

glm::vec3 TerrainChunk::calculateNormal(const HeightmapLoader& heightmap,
                                        int x, int z, float cellSize, float maxHeight)
{
    int width = heightmap.getWidth();
    int height = heightmap.getGridHeight();
    
    float hL = heightmap.getHeight(std::max(0, x - 1), z) * maxHeight;
    float hR = heightmap.getHeight(std::min(width - 1, x + 1), z) * maxHeight;
    float hD = heightmap.getHeight(x, std::max(0, z - 1)) * maxHeight;
    float hU = heightmap.getHeight(x, std::min(height - 1, z + 1)) * maxHeight;
    
    glm::vec3 normal(hL - hR, 2.0f * cellSize, hD - hU);
    return glm::normalize(normal);
}

glm::vec3 TerrainChunk::calculateTangent(const glm::vec3& normal)
{
    // For terrain, tangent is along +X axis (texture U direction)
    // Then orthogonalize with respect to normal using Gram-Schmidt
    glm::vec3 tangent(1.0f, 0.0f, 0.0f);
    
    // Gram-Schmidt orthogonalization
    tangent = tangent - glm::dot(tangent, normal) * normal;
    
    // Handle edge case where normal is parallel to X axis
    if (glm::length(tangent) < 0.001f)
    {
        tangent = glm::vec3(0.0f, 0.0f, 1.0f);
        tangent = tangent - glm::dot(tangent, normal) * normal;
    }
    
    return glm::normalize(tangent);
}

void TerrainChunk::render(int lodLevel)
{
    if (!m_generated) return;
    lodLevel = clampValue(lodLevel, 0, LOD_LEVELS - 1);
    m_lodMeshes[lodLevel].draw();
}

int TerrainChunk::getTriangleCount(int lodLevel) const
{
    lodLevel = clampValue(lodLevel, 0, LOD_LEVELS - 1);
    return m_triangleCounts[lodLevel];
}
