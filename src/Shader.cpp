#include "Shader.hpp"

#include <iostream>
#include <memory>
#include <vector>

#include "OpenGLExtensions.hpp"

// OpenGL Start
#include <GL/glew.h>
// glew must be imported before glfw3
#include <GLFW/glfw3.h>
// OpenGL End

std::unique_ptr<std::string>
    Shader::CompileShadelet(const ShadeletSource &shadeletSource, GLuint &id)
{
    id              = glCreateShader(shadeletSource.GetType());
    const char *src = shadeletSource.GetSource().c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    GLint result = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        GLint len = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);

        GLchar *msg = (GLchar *)malloc(len);
        glGetShaderInfoLog(id, len, &len, msg);

        auto return_msg = std::make_unique<std::string>(
            "Error in shader '" + shadeletSource.GetFullPath() + "':\n" +
            std::string((char *)msg));
        delete msg;
        return return_msg;
    }

    return std::unique_ptr<std::string>(nullptr);
}

std::unique_ptr<std::vector<Shader>>
    Shader::CompileShaders(const std::vector<ShaderSource> &          sources,
                           std::unique_ptr<std::vector<std::string>> &errors)
{

    // auto shaderIDs = std::vector<GLuint>();
    auto shaders = std::make_unique<std::vector<Shader>>();

    for (auto &source : sources)
    {
        GLuint      programID  = glCreateProgram();
        GLuint      shadeletID = 0;
        auto const &shadelets  = source.GetShadelets();
        // Compile and add each individual shadelet
        for (auto const &shadelet : shadelets)
        {
            auto compileResult = CompileShadelet(shadelet, shadeletID);
            if (compileResult == nullptr) // No error occurred
            {
                // shaderIDs.push_back(shadeletID);
                glAttachShader(programID, shadeletID);
                shaders->push_back(Shader(programID));

                glDeleteShader(shadeletID);
            }
            else
            {
                if (errors != nullptr)
                    errors->push_back(*compileResult);
                return shaders;
            }
        }
        glLinkProgram(programID);
        glValidateProgram(programID);
    }

    return shaders;
}

GLuint Shader::GetProgram() { return this->program; }

void Shader::SetInUse() { GLCall(glUseProgram(this->program)); }

GLint Shader::GetUniformLocation(const std::string &uniformName)
{
    GLCall(GLint val =
               glGetUniformLocation(this->program, uniformName.c_str()));
    return val;
}

Shader::Shader(GLuint programID) : program(programID) {}

Shader::~Shader() {}
