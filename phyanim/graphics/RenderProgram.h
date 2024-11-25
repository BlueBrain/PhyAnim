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

#ifndef __EXAMPLES_RENDERPROGRAM__
#define __EXAMPLES_RENDERPROGRAM__

#include <string>

namespace phyanim
{
namespace graphics
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
    int64_t pickingColor;

    uint64_t id;

private:
    uint64_t _vshader;
    uint64_t _gshader;
    uint64_t _fshader;
};

}  // namespace graphics
}  // namespace phyanim

#endif
