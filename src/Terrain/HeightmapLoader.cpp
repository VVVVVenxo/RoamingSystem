#include "HeightmapLoader.h"
#include <stb_image.h>
#include <iostream>
#include <algorithm>

HeightmapLoader::HeightmapLoader()
    : m_width(0)
    , m_height(0)
    , m_loaded(false)
{
}

HeightmapLoader::~HeightmapLoader()
{
}

bool HeightmapLoader::load(const std::string& path)
{
    m_heightData.clear();
    m_loaded = false;

    // Load image as grayscale
    int channels;
    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &channels, 1);

    if (!data)
    {
        std::cerr << "ERROR::HEIGHTMAP::FAILED_TO_LOAD: " << path << std::endl;
        return false;
    }

    std::cout << "Heightmap loaded: " << path << " (" << m_width << "x" << m_height << ")" << std::endl;

    // Convert to normalized float values (0-1)
    m_heightData.resize(m_width * m_height);
    for (int i = 0; i < m_width * m_height; i++)
    {
        m_heightData[i] = static_cast<float>(data[i]) / 255.0f;
    }

    stbi_image_free(data);
    m_loaded = true;

    return true;
}

float HeightmapLoader::getHeight(int x, int z) const
{
    if (!m_loaded)
    {
        return 0.0f;
    }

    // Clamp coordinates
    x = std::max(0, std::min(x, m_width - 1));
    z = std::max(0, std::min(z, m_height - 1));

    return m_heightData[z * m_width + x];
}

float HeightmapLoader::getHeightInterpolated(float x, float z) const
{
    if (!m_loaded)
    {
        return 0.0f;
    }

    // Clamp to valid range
    x = std::max(0.0f, std::min(x, static_cast<float>(m_width - 1)));
    z = std::max(0.0f, std::min(z, static_cast<float>(m_height - 1)));

    // Get integer and fractional parts
    int x0 = static_cast<int>(x);
    int z0 = static_cast<int>(z);
    int x1 = std::min(x0 + 1, m_width - 1);
    int z1 = std::min(z0 + 1, m_height - 1);

    float fx = x - static_cast<float>(x0);
    float fz = z - static_cast<float>(z0);

    // Bilinear interpolation
    float h00 = getHeight(x0, z0);
    float h10 = getHeight(x1, z0);
    float h01 = getHeight(x0, z1);
    float h11 = getHeight(x1, z1);

    float h0 = h00 * (1.0f - fx) + h10 * fx;
    float h1 = h01 * (1.0f - fx) + h11 * fx;

    return h0 * (1.0f - fz) + h1 * fz;
}
