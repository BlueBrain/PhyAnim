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

#ifndef __EXAMPLES_OVERLAP_COLLISIONS_APP__
#define __EXAMPLES_OVERLAP_COLLISIONS_APP__

#include <chrono>

#include "../common/GLFWApp.h"

using namespace phyanim;

namespace examples
{
class OverlapCollisionsApp : public GLFWApp
{
public:
    OverlapCollisionsApp(int argc, char** argv);

protected:
    void _actionLoop();

    geometry::MeshPtr _sliceMesh(geometry::HierarchicalAABBPtr tetAABB,
                                 const geometry::AxisAlignedBoundingBox& aabb,
                                 float stiffness,
                                 float density,
                                 float damping,
                                 float poissonRatio);

    void _setSurfaceNodes(geometry::MeshPtr mesh);

    void _mouseButtonCallback(GLFWwindow* window,
                              int button,
                              int action,
                              int mods);
};

}  // namespace examples

#endif
