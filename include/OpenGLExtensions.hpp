#pragma once
#ifndef OpenGLExtensions_hpp
#define OpenGLExtensions_hpp

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void GLClearError();

void GLCheckError(const char *func, const char *file, int line);

void PrintGLError(GLenum errorCode);

// Only enable this call if we are in debug mode
#ifndef NDEBUG
// Debug mode
#define GLCall(x)                                                              \
    GLClearError();                                                            \
    x;                                                                         \
    GLCheckError(#x, __FILE__, __LINE__)
#else
// Release mode
#define GLCall(x) x
#endif

#endif