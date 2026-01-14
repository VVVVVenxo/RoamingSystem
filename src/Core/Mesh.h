/**
 * @file Mesh.h
 * @brief Mesh management class (VAO/VBO/EBO wrapper)
 * @author LuNingfang
 */

#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <vector>
#include <cstddef>

enum class VertexAttribType
{
    Float,
    Int,
    UnsignedInt,
    Byte,
    UnsignedByte
};

struct VertexAttrib
{
    unsigned int index;
    int size;
    VertexAttribType type;
    bool normalized;
    size_t offset;
};

class VertexLayout
{
public:
    VertexLayout() : m_stride(0) {}

    void add(unsigned int index, int size, VertexAttribType type, bool normalized = false);
    const std::vector<VertexAttrib>& getAttribs() const { return m_attribs; }
    size_t getStride() const { return m_stride; }

    static VertexLayout positionOnly();
    static VertexLayout positionColor();
    static VertexLayout positionTexture();
    static VertexLayout positionColorTexture();
    static VertexLayout positionNormalTexture();
    static VertexLayout positionNormalTextureTangent();

private:
    std::vector<VertexAttrib> m_attribs;
    size_t m_stride;

    static size_t getTypeSize(VertexAttribType type);
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void setVertices(const void* data, size_t size, const VertexLayout& layout);
    void setIndices(const unsigned int* data, size_t count);
    void draw(GLenum mode = GL_TRIANGLES) const;
    void drawInstanced(unsigned int instanceCount, GLenum mode = GL_TRIANGLES) const;

    unsigned int getVAO() const { return m_vao; }
    unsigned int getVertexCount() const { return m_vertexCount; }
    unsigned int getIndexCount() const { return m_indexCount; }
    bool hasIndices() const { return m_ebo != 0; }
    bool isValid() const { return m_vao != 0; }

private:
    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_ebo;
    unsigned int m_vertexCount;
    unsigned int m_indexCount;

    void release();
};

#endif
