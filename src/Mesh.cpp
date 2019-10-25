#include "Mesh.hpp"
#include "OpenGLExtensions.hpp"

#include <iostream>

template <typename T>
std::size_t GetVecTypeSize(const std::vector<T> &vec)
{
    return sizeof(T);
}

template <typename T>
std::size_t VecTotalSize(const std::vector<T> &vec)
{
    return GetVecTypeSize(vec) * vec.size();
}

Mesh::Mesh() : vao(0), vbo(0), ibo(0) {}

Mesh::~Mesh() { ClearMesh(); }

void Mesh::CreateMesh(std::vector<Vertex> &&       vertices,
                      std::vector<std::uint32_t> &&indices)
{
    this->vertices = std::move(vertices);
    this->indices  = std::move(indices);
    // Vertex buffer
    GLCall(glGenBuffers(1, &this->vbo));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vbo));
    GLCall(glBufferData(GL_ARRAY_BUFFER, VecTotalSize(this->vertices),
                        this->vertices.data(), GL_STATIC_DRAW));

    // Setup vertex array object
    this->vao = Vertex::GenerateAttributes();

    // Index buffer
    GLCall(glGenBuffers(1, &this->ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, VecTotalSize(this->indices),
                        this->indices.data(), GL_STATIC_DRAW));

    // Unbind all buffers
    GLCall(glBindVertexArray(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Mesh::RenderMesh()
{
    if (this->vao == 0)
        std::cout << "vao == 0" << std::endl;
    if (this->vbo == 0)
        std::cout << "vbo == 0" << std::endl;
    if (this->ibo == 0)
        std::cout << "ibo == 0" << std::endl;

    GLCall(glBindVertexArray(vao));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo)); // Probably not needed
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));
    GLCall(glBindVertexArray(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); // Probably not needed
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Mesh::ClearMesh()
{
    glDeleteBuffers(1, &ibo);
    ibo = 0;
    glDeleteBuffers(1, &vbo);
    vbo = 0;
    glDeleteVertexArrays(1, &vao);
    vao = 0;
    indices.clear();
    vertices.clear();
}

Mesh::Mesh(Mesh &&other)
{
    vao       = other.vao;
    vbo       = other.vbo;
    ibo       = other.ibo;
    other.vao = other.vbo = other.ibo = 0; // Clear other

    vertices = std::move(other.vertices);
    indices  = std::move(other.indices);
}
