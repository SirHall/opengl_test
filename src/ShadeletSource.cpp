#include "ShadeletSource.hpp"

ShadeletSource::ShadeletSource(std::string src, GLenum tp, std::string path)
    : source(src), type(tp), fullPath(path)
{
}

ShadeletSource::~ShadeletSource() {}

const std::string &ShadeletSource::GetFullPath() const
{
    return this->fullPath;
}

const std::string &ShadeletSource::GetSource() const { return this->source; }

GLenum ShadeletSource::GetType() const { return this->type; }
