#include "Vertex.hpp"
#include "OpenGLExtensions.hpp"

// #include <iostream>

#include <cstdint>

Vertex::Vertex() {}

Vertex::Vertex(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z, GLfloat uv_x = 0.0f,
               GLfloat uv_y = 0.0f)
{
    position[0] = pos_x;
    position[1] = pos_y;
    position[2] = pos_z;

    uv[0] = uv_x;
    uv[1] = uv_y;
}

Vertex::~Vertex() {}

GLuint Vertex::GenerateAttributes()
{
    GLuint vao = 0;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));
    // Vertex attributes
    GLCall(glEnableVertexAttribArray(0));

    // std::cout << "o: " << offsetof(Vertex, position) << std::endl;

    GLuint currentIndex = 0;

    // Model position attribute
    GLCall(glVertexAttribPointer(
        currentIndex++, sizeof(position) / sizeof(GLfloat), GL_FLOAT, GL_FALSE,
        sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, position))));

    // UV Coordinate Attribute
    GLCall(glVertexAttribPointer(
        currentIndex++, sizeof(uv) / sizeof(GLfloat), GL_FLOAT, GL_FALSE,
        sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, uv))));

    return vao;
}