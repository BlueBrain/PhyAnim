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

#ifndef __EXAMPLES_SOMA_GENERATOR__
#define __EXAMPLES_SOMA_GENERATOR__

#include "Icosphere.h"

using namespace phyanim;

namespace examples
{
class SomaGenerator
{
public:
    SomaGenerator(geometry::Vec3 center,
                  float radius,
                  geometry::Nodes starts,
                  float dt = 0.01,
                  float stiffness = 1000.0,
                  float poissonRatio = 0.49);

    ~SomaGenerator(){};

    void anim(bool updateNodes = false);

    void pull(float alpha);

    geometry::MeshPtr animMesh;
    graphics::Mesh* renderMesh;

private:
    void _computeStartsNodes();

    void _fixCenterNodes(float radialDist = 0.5);

    void _updateNodes();

    geometry::Vec3 _center;
    float _radius;

    geometry::Nodes _starts;
    std::vector<geometry::Nodes> _startsNodes;
    std::vector<geometry::Vec3> _targets;
    float _dt;

    anim::ImplicitFEMSystem* _sys;
};

}  // namespace examples

#endif
