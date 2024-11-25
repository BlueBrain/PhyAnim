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

#ifndef __PHYANIM_EXPLICITMASSSPRINGSYSTEM__
#define __PHYANIM_EXPLICITMASSSPRINGSYSTEM__

#include "AnimSystem.h"

namespace phyanim
{
namespace anim
{
class ExplicitMassSpringSystem : public AnimSystem
{
public:
    ExplicitMassSpringSystem(float dt);

    virtual ~ExplicitMassSpringSystem(void);

    void step(geometry::Nodes& nodes,
              geometry::Edges& edges,
              geometry::AxisAlignedBoundingBox& limits,
              float ks,
              float kd);

protected:
    void _step(geometry::Mesh* mesh);
};

}  // namespace anim
}  // namespace phyanim

#endif