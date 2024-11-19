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
