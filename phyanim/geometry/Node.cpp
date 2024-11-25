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

#include "Node.h"

namespace phyanim
{
namespace geometry
{
Node::Node(Vec3 position,
           unsigned int id,
           float radius,
           Vec3 velocity,
           Vec3 force,
           float mass,
           bool surface,
           bool fix)
    : initPosition(position)
    , position(position)
    , radius(radius)
    , normal(Vec3(0, 0, 1))
    , id(id)
    , velocity(velocity)
    , force(force)
    , mass(mass)
    , surface(surface)
    , fix(fix)
    , isSoma(false)
    , anim(false)
    , collide(false)
{
}

Node::~Node() {}

bool Node::operator==(const Node& other_) const
{
    return (position == other_.position) && (id == other_.id);
}

bool Node::operator!=(const Node& other_) const { return !(*this == other_); }

void clearForce(Nodes& nodes)
{
    uint32_t size = nodes.size();
    for (uint32_t i = 0; i < size; ++i) nodes[i]->force = Vec3();
}

void clearCollision(Nodes& nodes)
{
    uint32_t size = nodes.size();

    for (uint32_t i = 0; i < size; ++i) nodes[i]->collide = false;
}

void clearVelocityIfNoColl(Nodes& nodes)
{
    uint32_t size = nodes.size();
    for (uint32_t i = 0; i < size; ++i)
        if (!nodes[i]->collide) nodes[i]->velocity = Vec3();
}

}  // namespace geometry
}  // namespace phyanim
