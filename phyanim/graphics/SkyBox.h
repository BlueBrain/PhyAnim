/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 *
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
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

#ifndef __EXAMPLES_SKYBOX__
#define __EXAMPLES_SKYBOX__

#include "Camera.h"
#include "RenderProgram.h"
#include "Texture.h"

namespace phyanim
{
namespace graphics
{
class SkyBox
{
public:
    SkyBox(const std::string& file);

    void render(Camera* camera);

protected:
    RenderProgram* _program;
    Texture* _texture;

    uint32_t _uView;
    uint32_t _uProj;
    uint32_t _vao;

    uint32_t _numIndices;
};

}  // namespace graphics
}  // namespace phyanim

#endif