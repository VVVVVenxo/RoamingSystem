/**
 * @file HeightmapLoader.h
 * @brief Heightmap loader class
 * @author LuNingfang
 */

#ifndef HEIGHTMAP_LOADER_H
#define HEIGHTMAP_LOADER_H

#include <string>
#include <vector>

class HeightmapLoader
{
public:
    HeightmapLoader();
    ~HeightmapLoader();

    bool load(const std::string& path);

    float getHeight(int x, int z) const;
    float getHeightInterpolated(float x, float z) const;

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
