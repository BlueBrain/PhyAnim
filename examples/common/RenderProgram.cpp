#include "RenderProgram.h"

#include <GL/glew.h>

#include <iostream>

namespace examples
{
RenderProgram::RenderProgram(const std::string& vShaderSource,
                             const std::string& gShaderSource,
                             const std::string& fShaderSource)
    : _id(0)
    , projviewmodelIndex(-1)
    , viewmodelIndex(-1)
    , pickingColor(-1)
    , _vshader(0)
    , _gshader(0)
    , _fshader(0)
{
    _createProgram(vShaderSource, gShaderSource, fShaderSource);
}

RenderProgram::~RenderProgram() { _deleteProgram(); }

void RenderProgram::use()
{
    if (_id > 0)
        glUseProgram(_id);
    else
    {
        std::cerr << "Rende program not initialized" << std::endl;
        exit(0);
    }
}

void RenderProgram::_createProgram(const std::string& vSource,
                                   const std::string& gSource,
                                   const std::string& fSource)
{
    _id = glCreateProgram();
    if (_id > 0)
    {
        if (!vSource.empty())
        {
            _vshader = _compileShader(vSource, GL_VERTEX_SHADER);
            if (_vshader == 0)
            {
                _deleteProgram();
                throw std::runtime_error("Compilation error in Vertex Shader");
            }
            glAttachShader(_id, _vshader);
        }
        if (!gSource.empty())
        {
            _gshader = _compileShader(gSource, GL_GEOMETRY_SHADER);
            if (_gshader == 0)
            {
                _deleteProgram();
                throw std::runtime_error(
                    "Compilation error in Geometry Shader");
            }
            glAttachShader(_id, _gshader);
        }
        if (!fSource.empty())
        {
            _fshader = _compileShader(fSource, GL_FRAGMENT_SHADER);
            if (_fshader == 0)
            {
                _deleteProgram();
                throw std::runtime_error(
                    "Compilation error in Fragment Shader");
            }
            glAttachShader(_id, _fshader);
        }
        glLinkProgram(_id);
        int32_t status;
        glGetProgramiv(_id, GL_LINK_STATUS, &status);
        if (status == 0)
        {
            _deleteProgram();
            throw std::runtime_error("Render program could not be linked");
        }
        projviewmodelIndex = glGetUniformLocation(_id, "projViewModel");
        viewmodelIndex = glGetUniformLocation(_id, "viewModel");
        pickingColor = glGetUniformLocation(_id, "pickingColor");
    }
}

void RenderProgram::_deleteProgram(void)
{
    if (_id > 0)
    {
        if (_vshader > 0)
        {
            glDetachShader(_id, _vshader);
            glDeleteShader(_vshader);
            _vshader = 0;
        }
        if (_gshader > 0)
        {
            glDetachShader(_id, _gshader);
            glDeleteShader(_gshader);
            _gshader = 0;
        }
        if (_fshader > 0)
        {
            glDetachShader(_id, _fshader);
            glDeleteShader(_fshader);
            _fshader = 0;
        }
        glDeleteProgram(_id);
        _id = 0;
        projviewmodelIndex = -1;
        viewmodelIndex = -1;
    }
}

uint64_t RenderProgram::_compileShader(const std::string& source, int type)
{
    unsigned int shader;
    shader = glCreateShader(type);
    const char* cSource = source.c_str();
    glShaderSource(shader, 1, &cSource, nullptr);

    int status;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char* infoLog = new char[length];
        glGetShaderInfoLog(shader, length, nullptr, infoLog);
        std::cerr << "Shader compile error: " << infoLog << std::endl;
        delete[] infoLog;
        return 0;
    }
    return shader;
}

}  // namespace examples
