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

#ifndef __EXAMPLES_DEMO_APP__
#define __EXAMPLES_DEMO_APP__

#include <phyanim/Phyanim.h>

#include "../common/GLFWApp.h"

namespace examples
{
class DemoApp : public GLFWApp
{
public:
    DemoApp(int argc, char** argv);

protected:
    void _actionLoop();

    void _checkCollisions();

    void _coloredMeshes();

    virtual void _keyCallback(GLFWwindow* window,
                              int key,
                              int scancode,
                              int action,
                              int mods);

    void _mouseButtonCallback(GLFWwindow* window,
                              int button,
                              int action,
                              int mods);

    void _mousePositionCallback(GLFWwindow* window, float xpos, float ypos);

    std::vector<std::string> _fileNames;
    phyanim::geometry::Mesh* _animMesh;
    phyanim::graphics::Mesh* _renderMesh;

    phyanim::graphics::ColorPalette _palette;
};

}  // namespace examples

#endif
