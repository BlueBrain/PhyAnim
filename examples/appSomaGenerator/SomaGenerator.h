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
