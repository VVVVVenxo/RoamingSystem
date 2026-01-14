/**
 * @file Texture.cpp
 * @brief 2D texture implementation
 * @author LuNingfang
 */

#include "Texture.h"
#include <stb_image.h>

Texture::Texture()
    : m_textureID(0)
    , m_width(0)
    , m_height(0)
    , m_channels(0)
    , m_loaded(false)
{
}

Texture::Texture(const std::string& path, bool flipVertically)
    : m_textureID(0)
    , m_width(0)
    , m_height(0)
    , m_channels(0)
    , m_loaded(false)
{
    load(path, flipVertically);
}

Texture::~Texture()
{
    release();
}

Texture::Texture(Texture&& other) noexcept
    : m_textureID(other.m_textureID)
    , m_width(other.m_width)
    , m_height(other.m_height)
    , m_channels(other.m_channels)
    , m_loaded(other.m_loaded)
{
    other.m_textureID = 0;
    other.m_loaded = false;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other)
    {
        release();

        m_textureID = other.m_textureID;
        m_width = other.m_width;
        m_height = other.m_height;
        m_channels = other.m_channels;
        m_loaded = other.m_loaded;

        other.m_textureID = 0;
        other.m_loaded = false;
    }
    return *this;
}

bool Texture::load(const std::string& path, bool flipVertically)
{
    release();

    stbi_set_flip_vertically_on_load(flipVertically);

    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
    if (!data)
    {
        std::cerr << "ERROR::TEXTURE::FAILED_TO_LOAD: " << path << std::endl;
        return false;
    }

    GLenum format = GL_RGB;
    GLenum internalFormat = GL_RGB;
    if (m_channels == 1)
    {
        format = GL_RED;
        internalFormat = GL_RED;
    }
    else if (m_channels == 3)
    {
        format = GL_RGB;
        internalFormat = GL_RGB;
    }
    else if (m_channels == 4)
    {
        format = GL_RGBA;
        internalFormat = GL_RGBA;
    }

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    m_loaded = true;
    std::cout << "Texture loaded: " << path << " (" << m_width << "x" << m_height << ", " << m_channels << " channels)" << std::endl;

    return true;
}

void Texture::bind(unsigned int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::release()
{
    if (m_textureID != 0)
    {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
    m_width = 0;
    m_height = 0;
    m_channels = 0;
    m_loaded = false;
}
