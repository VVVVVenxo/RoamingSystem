#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>
#include <iostream>

class Texture
{
public:
    Texture();
    Texture(const std::string& path, bool flipVertically = true);
    ~Texture();

    // Prevent copying
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Allow moving
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    bool load(const std::string& path, bool flipVertically = true);
    void bind(unsigned int unit = 0) const;
    void unbind() const;

    unsigned int getID() const { return m_textureID; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getChannels() const { return m_channels; }
    bool isLoaded() const { return m_loaded; }

private:
    unsigned int m_textureID;
    int m_width;
    int m_height;
    int m_channels;
    bool m_loaded;

    void release();
};

#endif
