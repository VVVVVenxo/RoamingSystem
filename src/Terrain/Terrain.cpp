#include "Terrain.h"
#include <iostream>
#include <vector>

Terrain::Terrain()
    : m_size(100.0f)
    , m_maxHeight(20.0f)
    , m_generated(false)
    , m_vertexCount(0)
    , m_triangleCount(0)
{
}

Terrain::~Terrain()
{
}

bool Terrain::generate(const std::string& heightmapPath, float size, float maxHeight)
{
    m_size = size;
    m_maxHeight = maxHeight;
    m_generated = false;

    if (!m_heightmap.load(heightmapPath))
    {
        std::cerr << "ERROR::TERRAIN::FAILED_TO_LOAD_HEIGHTMAP" << std::endl;
        return false;
    }

    generateMesh();
    m_generated = true;

    std::cout << "Terrain generated: " << m_heightmap.getWidth() << "x" << m_heightmap.getGridHeight()
              << " (size: " << m_size << ", maxHeight: " << m_maxHeight << ")" << std::endl;

    return true;
}

void Terrain::generateMesh()
{
    int width = m_heightmap.getWidth();
    int height = m_heightmap.getGridHeight();

    // Vertex data: position(3) + normal(3) + texcoord(2)
    std::vector<float> vertices;
    vertices.reserve(width * height * 8);

    // Generate vertices
    for (int z = 0; z < height; z++)
    {
        for (int x = 0; x < width; x++)
        {
            // Calculate world position
            float worldX = (static_cast<float>(x) / static_cast<float>(width - 1) - 0.5f) * m_size;
            float worldZ = (static_cast<float>(z) / static_cast<float>(height - 1) - 0.5f) * m_size;
            float worldY = m_heightmap.getHeight(x, z) * m_maxHeight;

            // Calculate normal
            glm::vec3 normal = calculateNormal(x, z);

            // Calculate texture coordinates
            float u = static_cast<float>(x) / static_cast<float>(width - 1);
            float v = static_cast<float>(z) / static_cast<float>(height - 1);

            // Add vertex data
            vertices.push_back(worldX);
            vertices.push_back(worldY);
            vertices.push_back(worldZ);
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    // Generate indices
    std::vector<unsigned int> indices;
    indices.reserve((width - 1) * (height - 1) * 6);

    for (int z = 0; z < height - 1; z++)
    {
        for (int x = 0; x < width - 1; x++)
        {
            unsigned int topLeft = z * width + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * width + x;
            unsigned int bottomRight = bottomLeft + 1;

            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    // Create mesh with position + normal + texcoord layout
    m_mesh.setVertices(vertices.data(), vertices.size() * sizeof(float), VertexLayout::positionNormalTexture());
    m_mesh.setIndices(indices.data(), indices.size());

    // Store counts for performance stats
    m_vertexCount = static_cast<int>(vertices.size() / 8);
    m_triangleCount = static_cast<int>(indices.size() / 3);

    std::cout << "Terrain mesh: " << m_vertexCount << " vertices, " << m_triangleCount << " triangles" << std::endl;
}

glm::vec3 Terrain::calculateNormal(int x, int z)
{
    int width = m_heightmap.getWidth();
    int height = m_heightmap.getGridHeight();

    // Get heights of neighboring vertices
    float hL = m_heightmap.getHeight(x > 0 ? x - 1 : x, z) * m_maxHeight;
    float hR = m_heightmap.getHeight(x < width - 1 ? x + 1 : x, z) * m_maxHeight;
    float hD = m_heightmap.getHeight(x, z > 0 ? z - 1 : z) * m_maxHeight;
    float hU = m_heightmap.getHeight(x, z < height - 1 ? z + 1 : z) * m_maxHeight;

    // Calculate step size in world units
    float stepX = m_size / static_cast<float>(width - 1);
    float stepZ = m_size / static_cast<float>(height - 1);

    // Calculate normal using central differences
    glm::vec3 normal;
    normal.x = (hL - hR) / (2.0f * stepX);
    normal.y = 1.0f;
    normal.z = (hD - hU) / (2.0f * stepZ);

    return glm::normalize(normal);
}

void Terrain::render(Shader& shader)
{
    if (!m_generated)
    {
        return;
    }

    shader.use();
    m_mesh.draw();
}

float Terrain::getHeightAt(float worldX, float worldZ) const
{
    if (!m_generated)
    {
        return 0.0f;
    }

    // Convert world coordinates to heightmap coordinates
    float halfSize = m_size * 0.5f;
    
    // Normalize to 0-1 range
    float normalizedX = (worldX + halfSize) / m_size;
    float normalizedZ = (worldZ + halfSize) / m_size;

    // Convert to heightmap pixel coordinates
    float pixelX = normalizedX * static_cast<float>(m_heightmap.getWidth() - 1);
    float pixelZ = normalizedZ * static_cast<float>(m_heightmap.getGridHeight() - 1);

    // Get interpolated height
    return m_heightmap.getHeightInterpolated(pixelX, pixelZ) * m_maxHeight;
}
