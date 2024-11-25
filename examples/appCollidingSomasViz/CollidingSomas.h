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
