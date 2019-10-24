#pragma once
#ifndef Vertex_hpp
#define Vertex_hpp

#include <cstdint>

#include <GL/glew.h>

struct Vertex
{
public:
    Vertex();
    Vertex(GLfloat x, GLfloat y, GLfloat z);
    ~Vertex();
    static GLuint GenerateAttributes();
    GLfloat       position[3] = {0.0f, 0.0f, 0.0f};
};

#endif
