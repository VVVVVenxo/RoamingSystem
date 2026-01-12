#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <glad/glad.h>
#include <string>
#include <vector>

class Cubemap
{
public:
    Cubemap();
    ~Cubemap();

    // Prevent copying
    Cubemap(const Cubemap&) = delete;
    Cubemap& operator=(const Cubemap&) = delete;

    // Allow moving
    Cubemap(Cubemap&& other) noexcept;
    Cubemap& operator=(Cubemap&& other) noexcept;

    // Load from 6 images (order: +X, -X, +Y, -Y, +Z, -Z)
    bool load(const std::vector<std::string>& faces);

    // Load from directory (looks for right/left/top/bottom/front/back.jpg)
    bool loadFromDirectory(const std::string& directory, const std::string& extension = ".jpg");

    void bind(unsigned int unit = 0) const;
    void unbind() const;

    unsigned int getID() const { return m_textureID; }
    bool isLoaded() const { return m_loaded; }

private:
    unsigned int m_textureID;
    bool m_loaded;

    void release();
};

#endif
