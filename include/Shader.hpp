#pragma once
#ifndef shader_hpp
#define shader_hpp

#include "ShaderSource.hpp"

#include <memory>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Shader
{
private:
    GLuint program;

    static std::unique_ptr<std::string>
        CompileShadelet(const ShadeletSource &shadeletSource, GLuint &id);

    Shader(GLuint programID);

public:
    ~Shader();

    static std::unique_ptr<std::vector<Shader>>
        CompileShaders(const std::vector<ShaderSource> &          sources,
                       std::unique_ptr<std::vector<std::string>> &errors);

    GLuint GetProgram();

    void SetInUse();

    GLint GetUniformLocation(const std::string &uniformName);
};

#endif
