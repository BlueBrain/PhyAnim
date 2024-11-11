/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/PhyAnim>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __EXAMPLES_RENDERPROGRAM__
#define __EXAMPLES_RENDERPROGRAM__

#include <string>

namespace examples
{

class RenderProgram
{
  public:

    RenderProgram(const std::string& vShaderSource,
                  const std::string& gShaderSource,
                  const std::string& fShaderSource);

    virtual ~RenderProgram(void);

    void use(void);

  private:

    void _createProgram(const std::string& vShaderSource,
                        const std::string& gShaderSource,
                        const std::string& fShaderSource);
    
    void _deleteProgram(void);
    
    uint64_t _compileShader(const std::string& source, int type);

  public:

    int64_t projviewmodelIndex;
    int64_t viewmodelIndex;

  private:

    uint64_t _id;
    uint64_t _vshader;
    uint64_t _gshader;
    uint64_t _fshader;
};

}

#endif 
