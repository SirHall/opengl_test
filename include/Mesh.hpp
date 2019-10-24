#pragma once
#ifndef Mesh_hpp
#define Mesh_hpp

#include "Vertex.hpp"

#include <cstdint>
#include <vector>

#include <GL/glew.h>

class Mesh
{
private:
    GLuint vao, // Vertex Array Object
        vbo,    // Vertex Buffer Object
        ibo;    // Index Buffer Object
    std::vector<Vertex>        vertices;
    std::vector<std::uint32_t> indices;

public:
    Mesh();
    ~Mesh();

    void CreateMesh(std::vector<Vertex> &&       vertices,
                    std::vector<std::uint32_t> &&indices);
    void RenderMesh();
    void ClearMesh();
};

#endif
