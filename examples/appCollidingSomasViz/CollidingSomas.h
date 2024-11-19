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

#ifndef __EXAMPLES_COLLIDING_SOMAS__
#define __EXAMPLES_COLLIDING_SOMAS__

#include "../common/CollisionSolver.h"
#include "../common/GLFWApp.h"
#include "../common/Morpho.h"

using namespace phyanim;

namespace examples
{
class CollidingSomas : public GLFWApp
{
public:
    CollidingSomas(int argc, char** argv);

    ~CollidingSomas(){};

protected:
    void _actionLoop();

    void _setMeshes(std::vector<geometry::Edges>& edgesSet);

    uint32_t _solveCollisions(geometry::HierarchicalAABBs& aabbs,
                              std::vector<geometry::Edges>& edgesSet,
                              std::vector<geometry::Nodes>& nodesSet);

    void _mouseButtonCallback(GLFWwindow* window,
                              int button,
                              int action,
                              int mods);

private:
    graphics::ColorPalette* _palette;

    CollisionSolver* _solver;

    float _dt;
    float _ksc;
};

}  // namespace examples

#endif
