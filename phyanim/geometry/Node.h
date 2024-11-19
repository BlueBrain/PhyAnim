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

#ifndef __PHYANIM_NODE__
#define __PHYANIM_NODE__

#include <vector>

#include "Math.h"

namespace phyanim
{
namespace geometry
{
class Node;

typedef Node* NodePtr;

typedef std::vector<NodePtr> Nodes;

class Node
{
public:
    Node(Vec3 position_,
         unsigned int id_ = 0,
         float radius = 1.0,
         Vec3 velocity_ = Vec3(),
         Vec3 force_ = Vec3(),
         float mass_ = 1.0,
         bool surface_ = false,
         bool fixed_ = false);

    virtual ~Node(void);

    bool operator==(const Node& other_) const;

    bool operator!=(const Node& other_) const;

    Vec3 initPosition;

    Vec3 position;

    float radius;

    Vec3 normal;

    Vec3 velocity;

    Vec3 force;

    float mass;

    unsigned int id;

    bool surface;

    bool fix;

    bool isSoma;

    bool anim;

    bool collide;
};

void clearForce(Nodes& nodes);

void clearCollision(Nodes& nodes);

void clearVelocityIfNoColl(Nodes& nodes);

}  // namespace geometry
}  // namespace phyanim

#endif
