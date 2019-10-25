#pragma once
#ifndef Vertex_hpp
#define Vertex_hpp

#include <cstdint>

#include <GL/glew.h>

struct Vertex
{
public:
    Vertex();
    Vertex(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z, GLfloat uv_x,
           GLfloat uv_y);
    ~Vertex();
    static GLuint GenerateAttributes();
    GLfloat       position[3] = {0.0f, 0.0f, 0.0f};
    GLfloat       uv[2]       = {0.0f, 0.0f};
};

#endif
