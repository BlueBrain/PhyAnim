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

#ifndef __PHYANIM_ANIMSYSTEM__
#define __PHYANIM_ANIMSYSTEM__

#include "CollisionDetection.h"

namespace phyanim
{
namespace anim
{
#define THRESHOLD 0.01f

class AnimSystem
{
public:
    AnimSystem(float dt);

    virtual ~AnimSystem(void);

    void step(geometry::Mesh* mesh);

    void step(geometry::Meshes meshes);

    virtual void preprocessMesh(geometry::Mesh* mesh){};

    void preprocessMesh(geometry::Meshes meshes);

protected:
    virtual void _step(geometry::Mesh* mesh) = 0;

    void _addGravity(geometry::Nodes& nodes);

    void _update(geometry::Nodes& nodes);

    void _updateIfCollide(geometry::Nodes& nodes);

public:
    bool gravity;
    bool inertia;

protected:
    geometry::Meshes _meshes;

    float _dt;
};

}  // namespace anim
}  // namespace phyanim

#endif
