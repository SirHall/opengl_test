#pragma once
#ifndef ShaderSource_hpp
#define ShaderSource_hpp

#include "ShadeletSource.hpp"

#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class ShaderSource
{
  private:
    std::string                 name;
    std::vector<ShadeletSource> shadelets;

    static bool HasEnding(std::string const &fullString,
                          std::string const &ending);

    static bool GetShaderType(std::string const &typeString, GLenum &type);

  public:
    ShaderSource(std::string name);
    ~ShaderSource();

    const std::string &GetName() const;

    static std::vector<ShaderSource>
        ReadShaderSources(std::string directoryPath);

    void AddShadelet(ShadeletSource shadelet);

    std::vector<ShadeletSource> const &GetShadelets() const;
};

#endif
