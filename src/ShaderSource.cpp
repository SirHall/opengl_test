#include "ShaderSource.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

ShaderSource::ShaderSource(std::string name) { this->name = name; }

ShaderSource::~ShaderSource() {}

const std::string &ShaderSource::GetName() const { return this->name; }

std::vector<ShaderSource>
    ShaderSource::ReadShaderSources(std::string directoryPath)
{
    auto sources = std::vector<ShaderSource>();

    if (!std::filesystem::is_directory(directoryPath))
        return sources; // Just return the vector

    for (auto &dir :
         std::filesystem::recursive_directory_iterator(directoryPath))
    {
        std::error_code error;
        if (dir.exists(error))
        {
            if (error)
            {
                std::cout << error.message() << std::endl;
                return sources;
            }
            if (dir.is_regular_file(error))
            {
                if (error)
                {
                    std::cout << error.message() << std::endl;
                    return sources;
                }

                if (HasEnding(dir.path().generic_string(), ".json"))
                {
                    // std::string cleanName = "";

                    namespace pt = boost::property_tree;
                    auto root    = pt::ptree();
                    try
                    {
                        pt::read_json(dir.path(), root);
                        // Determine if this JSON file represents a shader
                        // program
                        boost::optional<std::string> jsonType =
                            root.get_optional<std::string>("type");
                        if (!jsonType.has_value())
                        {
                            std::cerr << "JSON file has no type" << std::endl;
                            continue;
                        }
                        if (jsonType.get() != "shader")
                            continue; // This is not a shader, ignore it
                        // Get the shader programs name
                        boost::optional<std::string> shaderName =
                            root.get_optional<std::string>("name");
                        if (!shaderName.has_value())
                        {
                            std::cerr
                                << "All shaders must have a 'name' field in "
                                   "the root"
                                << std::endl;
                            continue;
                        }

                        auto shaderJSONSources =
                            root.get_child_optional("sources");
                        if (!shaderJSONSources.has_value())
                        {
                            std::cerr
                                << "A shader must have sources to create the "
                                   "shader from"
                                << std::endl;
                            continue;
                        }
                        // The format of the shader source data
                        struct JSONShaderSourceData
                        {
                            std::string dir;
                            std::string type;
                        };
                        auto shaderSource = ShaderSource(shaderName.get());
                        // Iterate over shadelets
                        for (auto &shadeletData : shaderJSONSources.get())
                        {
                            auto shadeletDirectory =
                                shadeletData.second.get_optional<std::string>(
                                    "path");
                            auto shadeletTypeStr =
                                shadeletData.second.get_optional<std::string>(
                                    "type");
                            if (!shadeletDirectory.has_value())
                            {
                                std::cerr
                                    << "Every shader source must have a "
                                       "directory "
                                       "'path' value to tell the loader where "
                                       "the "
                                       "source file is located relative to the "
                                       "shader JSON data file."
                                    << std::endl;
                                continue;
                            }
                            if (!shadeletTypeStr.has_value())
                            {
                                std::cerr
                                    << "Every shader source must have a type "
                                       "'type' specified"
                                    << std::endl;
                                continue;
                            }

                            // Find the source file and read it into the shader
                            // source

                            auto shadeletFullPath =
                                dir.path().parent_path().append(
                                    shadeletDirectory.get());

                            std::cout << "Reading: "
                                      << shadeletFullPath.generic_string()
                                      << std::endl;

                            auto shadeletSourceFile =
                                std::ifstream(shadeletFullPath);
                            auto shadeletContents =
                                std::string(std::istreambuf_iterator<char>(
                                                shadeletSourceFile),
                                            std::istreambuf_iterator<char>());

                            shadeletSourceFile.close();

                            GLenum shadeletType = 0;
                            if (!GetShaderType(shadeletTypeStr.get(),
                                               shadeletType))
                            {
                                std::cerr
                                    << "Shadelet must have a valid type. Got: '"
                                    << shadeletTypeStr << "'" << std::endl;
                                continue;
                            }
                            shaderSource.AddShadelet(
                                ShadeletSource(shadeletContents, shadeletType,
                                               shadeletFullPath));
                        }
                        sources.push_back(shaderSource);
                    }
                    catch (std::exception &e)
                    {
                        std::cerr << e.what() << std::endl;
                        continue;
                    }
                }
            }
        }
        else
        {
            std::cout << "Shader directory '" << directoryPath
                      << "' does not exist" << std::endl;
        }
    }
    std::cout << "Finished reading shaders" << std::endl;
    return sources;
}

bool ShaderSource::HasEnding(std::string const &fullString,
                             std::string const &ending)
{
    if (fullString.length() >= ending.length())
        return (0 == fullString.compare(fullString.length() - ending.length(),
                                        ending.length(), ending));
    else
        return false;
}

bool ShaderSource::GetShaderType(std::string const &typeString, GLenum &type)
{
    // List of shader type extensions and their corresponding GL shader types
    const auto shaderTypes = std::vector<std::pair<std::string, GLenum>>(
        {{"vertex", GL_VERTEX_SHADER},
         {"fragment", GL_FRAGMENT_SHADER},
         {"geometry", GL_GEOMETRY_SHADER}});

    for (auto shaderType : shaderTypes)
    {
        if (typeString == shaderType.first)
        {
            type = shaderType.second;
            return true;
        }
    }
    return false;
}

void ShaderSource::AddShadelet(ShadeletSource shadelet)
{
    shadelets.push_back(shadelet);
}

std::vector<ShadeletSource> const &ShaderSource::GetShadelets() const
{
    return shadelets;
}