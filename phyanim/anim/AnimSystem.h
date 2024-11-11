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
