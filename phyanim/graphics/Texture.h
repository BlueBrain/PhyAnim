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

#ifndef __EXAMPLES_TEXTURE__
#define __EXAMPLES_TEXTURE__

#include <iostream>

namespace phyanim
{
namespace graphics
{
class Texture
{
public:
    Texture(const std::string& fileName = "",
            uint32_t width = 600,
            uint32_t height = 600);

    void loadPng(const std::string& fileName);

    uint32_t id;

private:
    uint32_t _width;
    uint32_t _height;
};

}  // namespace graphics
}  // namespace phyanim

#endif