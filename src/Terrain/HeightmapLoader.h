#ifndef HEIGHTMAP_LOADER_H
#define HEIGHTMAP_LOADER_H

#include <string>
#include <vector>

class HeightmapLoader
{
public:
    HeightmapLoader();
    ~HeightmapLoader();

    // Load heightmap from file (grayscale image)
    bool load(const std::string& path);

    // Get height value at integer coordinates (normalized 0-1)
    float getHeight(int x, int z) const;

    // Get height value with bilinear interpolation (normalized 0-1)
    float getHeightInterpolated(float x, float z) const;

    // Dimensions
    int getWidth() const { return m_width; }
    int getGridHeight() const { return m_height; }

    bool isLoaded() const { return m_loaded; }

private:
    std::vector<float> m_heightData;
    int m_width;
    int m_height;
    bool m_loaded;
};

#endif
