#include "Vertex.hpp"
#include "OpenGLExtensions.hpp"

// #include <iostream>

#include <cstdint>

Vertex::Vertex() {}

Vertex::Vertex(GLfloat x, GLfloat y, GLfloat z)
{
    position[0] = x;
    position[1] = y;
    position[2] = z;
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

    GLCall(glVertexAttribPointer(
        currentIndex++, sizeof(position) / sizeof(GLfloat), GL_FLOAT, GL_FALSE,
        sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, position))));

    return vao;
}