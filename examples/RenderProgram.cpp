/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>

#include <GL/glew.h>

#include "RenderProgram.h"

namespace examples
{

RenderProgram::RenderProgram(const std::string& vShaderSource,
                             const std::string& gShaderSource,
                             const std::string& fShaderSource)
    : _id(0)
    , projviewmodelIndex(-1)
    , viewmodelIndex(-1)
    , _vshader(0)
    , _gshader(0)
    , _fshader(0)
{
    _createProgram(vShaderSource, gShaderSource, fShaderSource);
}


RenderProgram::~RenderProgram()
{
    _deleteProgram();
}

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
        std::cout << "program created" << std::endl;
        if (!vSource.empty())
        {
            _vshader = _compileShader(vSource, GL_VERTEX_SHADER);
            if (_vshader == 0)
            {
                _deleteProgram();
                std::cerr << "vshader compilation error" << std::endl;
                return;
            }
            glAttachShader(_id, _vshader);
        }
        if (!gSource.empty())
        {
            _gshader = _compileShader(gSource, GL_GEOMETRY_SHADER);
            if (_gshader == 0)
            {
                _deleteProgram();
                
                std::cerr << "gshader compilation error" << std::endl;
                return;
            }
            glAttachShader(_id, _gshader);
        }
        if (!fSource.empty())
        {
            _fshader = _compileShader(fSource, GL_FRAGMENT_SHADER);
            if (_fshader == 0)
            {
                
                std::cerr << "fshader compilation error" << std::endl;
                _deleteProgram();
                return;
            }
            glAttachShader(_id, _fshader);
        }
        glLinkProgram(_id);
        int32_t status;
        glGetProgramiv(_id, GL_LINK_STATUS, &status);
        if (status == 0)
        {
            _deleteProgram();
                std::cerr << "program link error" << std::endl;
                return;
        }
        projviewmodelIndex = glGetUniformLocation(_id, "projViewModel");
        viewmodelIndex = glGetUniformLocation(_id, "viewModel");
        std::cout << projviewmodelIndex << "   " << viewmodelIndex << std::endl;
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

}
