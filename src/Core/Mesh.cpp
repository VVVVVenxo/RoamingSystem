#include "Mesh.h"

// ============================================
// VertexLayout Implementation
// ============================================

size_t VertexLayout::getTypeSize(VertexAttribType type)
{
    switch (type)
    {
    case VertexAttribType::Float:       return sizeof(float);
    case VertexAttribType::Int:         return sizeof(int);
    case VertexAttribType::UnsignedInt: return sizeof(unsigned int);
    case VertexAttribType::Byte:        return sizeof(char);
    case VertexAttribType::UnsignedByte:return sizeof(unsigned char);
    default:                            return sizeof(float);
    }
}

void VertexLayout::add(unsigned int index, int size, VertexAttribType type, bool normalized)
{
    VertexAttrib attrib;
    attrib.index = index;
    attrib.size = size;
    attrib.type = type;
    attrib.normalized = normalized;
    attrib.offset = m_stride;

    m_attribs.push_back(attrib);
    m_stride += size * getTypeSize(type);
}

VertexLayout VertexLayout::positionOnly()
{
    VertexLayout layout;
    layout.add(0, 3, VertexAttribType::Float); // position
    return layout;
}

VertexLayout VertexLayout::positionColor()
{
    VertexLayout layout;
    layout.add(0, 3, VertexAttribType::Float); // position
    layout.add(1, 3, VertexAttribType::Float); // color
    return layout;
}

VertexLayout VertexLayout::positionTexture()
{
    VertexLayout layout;
    layout.add(0, 3, VertexAttribType::Float); // position
    layout.add(1, 2, VertexAttribType::Float); // texcoord
    return layout;
}

VertexLayout VertexLayout::positionColorTexture()
{
    VertexLayout layout;
    layout.add(0, 3, VertexAttribType::Float); // position
    layout.add(1, 3, VertexAttribType::Float); // color
    layout.add(2, 2, VertexAttribType::Float); // texcoord
    return layout;
}

VertexLayout VertexLayout::positionNormalTexture()
{
    VertexLayout layout;
    layout.add(0, 3, VertexAttribType::Float); // position
    layout.add(1, 3, VertexAttribType::Float); // normal
    layout.add(2, 2, VertexAttribType::Float); // texcoord
    return layout;
}

VertexLayout VertexLayout::positionNormalTextureTangent()
{
    VertexLayout layout;
    layout.add(0, 3, VertexAttribType::Float); // position
    layout.add(1, 3, VertexAttribType::Float); // normal
    layout.add(2, 2, VertexAttribType::Float); // texcoord
    layout.add(3, 3, VertexAttribType::Float); // tangent
    return layout;
}

// ============================================
// Mesh Implementation
// ============================================

Mesh::Mesh()
    : m_vao(0)
    , m_vbo(0)
    , m_ebo(0)
    , m_vertexCount(0)
    , m_indexCount(0)
{
}

Mesh::~Mesh()
{
    release();
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_vao(other.m_vao)
    , m_vbo(other.m_vbo)
    , m_ebo(other.m_ebo)
    , m_vertexCount(other.m_vertexCount)
    , m_indexCount(other.m_indexCount)
{
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_vertexCount = 0;
    other.m_indexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other)
    {
        release();

        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;
        m_vertexCount = other.m_vertexCount;
        m_indexCount = other.m_indexCount;

        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;
        other.m_vertexCount = 0;
        other.m_indexCount = 0;
    }
    return *this;
}

void Mesh::setVertices(const void* data, size_t size, const VertexLayout& layout)
{
    // Release previous resources
    release();

    // Generate VAO and VBO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    // Bind VAO
    glBindVertexArray(m_vao);

    // Upload vertex data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    // Configure vertex attributes
    for (const auto& attrib : layout.getAttribs())
    {
        GLenum glType = GL_FLOAT;
        switch (attrib.type)
        {
        case VertexAttribType::Float:       glType = GL_FLOAT; break;
        case VertexAttribType::Int:         glType = GL_INT; break;
        case VertexAttribType::UnsignedInt: glType = GL_UNSIGNED_INT; break;
        case VertexAttribType::Byte:        glType = GL_BYTE; break;
        case VertexAttribType::UnsignedByte:glType = GL_UNSIGNED_BYTE; break;
        }

        glVertexAttribPointer(
            attrib.index,
            attrib.size,
            glType,
            attrib.normalized ? GL_TRUE : GL_FALSE,
            static_cast<GLsizei>(layout.getStride()),
            reinterpret_cast<void*>(attrib.offset)
        );
        glEnableVertexAttribArray(attrib.index);
    }

    // Calculate vertex count
    if (layout.getStride() > 0)
    {
        m_vertexCount = static_cast<unsigned int>(size / layout.getStride());
    }

    // Unbind
    glBindVertexArray(0);
}

void Mesh::setIndices(const unsigned int* data, size_t count)
{
    if (m_vao == 0)
    {
        return;
    }

    glBindVertexArray(m_vao);

    // Generate EBO if not exists
    if (m_ebo == 0)
    {
        glGenBuffers(1, &m_ebo);
    }

    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);

    m_indexCount = static_cast<unsigned int>(count);

    glBindVertexArray(0);
}

void Mesh::draw(GLenum mode) const
{
    if (m_vao == 0)
    {
        return;
    }

    glBindVertexArray(m_vao);

    if (hasIndices())
    {
        glDrawElements(mode, m_indexCount, GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(mode, 0, m_vertexCount);
    }

    glBindVertexArray(0);
}

void Mesh::drawInstanced(unsigned int instanceCount, GLenum mode) const
{
    if (m_vao == 0)
    {
        return;
    }

    glBindVertexArray(m_vao);

    if (hasIndices())
    {
        glDrawElementsInstanced(mode, m_indexCount, GL_UNSIGNED_INT, 0, instanceCount);
    }
    else
    {
        glDrawArraysInstanced(mode, 0, m_vertexCount, instanceCount);
    }

    glBindVertexArray(0);
}

void Mesh::release()
{
    if (m_ebo != 0)
    {
        glDeleteBuffers(1, &m_ebo);
        m_ebo = 0;
    }
    if (m_vbo != 0)
    {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    m_vertexCount = 0;
    m_indexCount = 0;
}
