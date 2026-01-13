#include "Cubemap.h"
#include <stb_image.h>
#include <iostream>

Cubemap::Cubemap()
    : m_textureID(0)
    , m_loaded(false)
{
}

Cubemap::~Cubemap()
{
    release();
}

Cubemap::Cubemap(Cubemap&& other) noexcept
    : m_textureID(other.m_textureID)
    , m_loaded(other.m_loaded)
{
    other.m_textureID = 0;
    other.m_loaded = false;
}

Cubemap& Cubemap::operator=(Cubemap&& other) noexcept
{
    if (this != &other)
    {
        release();
        m_textureID = other.m_textureID;
        m_loaded = other.m_loaded;
        other.m_textureID = 0;
        other.m_loaded = false;
    }
    return *this;
}

void Cubemap::release()
{
    if (m_textureID != 0)
    {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
    m_loaded = false;
}

bool Cubemap::load(const std::vector<std::string>& faces)
{
    if (faces.size() != 6)
    {
        std::cerr << "ERROR::CUBEMAP::INVALID_FACE_COUNT: Expected 6, got " << faces.size() << std::endl;
        return false;
    }

    release();

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

    // Don't flip cubemap textures vertically
    stbi_set_flip_vertically_on_load(false);

    int width, height, channels;
    for (unsigned int i = 0; i < 6; i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
        if (data)
        {
            GLenum format = GL_RGB;
            if (channels == 1)
                format = GL_RED;
            else if (channels == 3)
                format = GL_RGB;
            else if (channels == 4)
                format = GL_RGBA;

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
            std::cout << "Cubemap face loaded: " << faces[i] << " (" << width << "x" << height << ")" << std::endl;
        }
        else
        {
            std::cerr << "ERROR::CUBEMAP::FAILED_TO_LOAD: " << faces[i] << std::endl;
            stbi_image_free(data);
            release();
            return false;
        }
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    m_loaded = true;

    std::cout << "Cubemap loaded successfully" << std::endl;
    return true;
}

bool Cubemap::loadFromDirectory(const std::string& directory, const std::string& extension)
{
    // Order: +X, -X, +Y, -Y, +Z, -Z
    // Try short naming first (px/nx)
    std::vector<std::string> facesShort = {
        directory + "/px" + extension,
        directory + "/nx" + extension,
        directory + "/py" + extension,
        directory + "/ny" + extension,
        directory + "/pz" + extension,
        directory + "/nz" + extension
    };

    if (load(facesShort))
    {
        return true;
    }

    // Try OpenGL standard naming (posx/negx)
    std::vector<std::string> facesGL = {
        directory + "/posx" + extension,
        directory + "/negx" + extension,
        directory + "/posy" + extension,
        directory + "/negy" + extension,
        directory + "/posz" + extension,
        directory + "/negz" + extension
    };

    if (load(facesGL))
    {
        return true;
    }

    // Fallback to descriptive naming (right/left/top/bottom/front/back)
    std::vector<std::string> facesDesc = {
        directory + "/right" + extension,
        directory + "/left" + extension,
        directory + "/top" + extension,
        directory + "/bottom" + extension,
        directory + "/front" + extension,
        directory + "/back" + extension
    };

    return load(facesDesc);
}

void Cubemap::bind(unsigned int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
}

void Cubemap::unbind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
