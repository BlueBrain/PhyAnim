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

#include "AnimSystem.h"

#include <iostream>

namespace phyanim
{
namespace anim
{
AnimSystem::AnimSystem(float dt) : gravity(true), inertia(true), _dt(dt) {}

AnimSystem::~AnimSystem() {}

void AnimSystem::step(geometry::Mesh* mesh)
{
    if (gravity)
    {
        geometry::Vec3 g = geometry::Vec3();
        g.y = -9.8f;

        for (unsigned int i = 0; i < mesh->nodes.size(); i++)
        {
            auto node = mesh->nodes[i];
            node->force += g * node->mass;
        }
    }
    for (unsigned int i = 0; i < mesh->nodes.size(); i++)
    {
        auto node = mesh->nodes[i];
        node->anim = true;
    }
    _step(mesh);
}

void AnimSystem::step(geometry::Meshes meshes)
{
    if (gravity)
    {
        geometry::Vec3 g = geometry::Vec3();
        g.y = -9.8f;
        for (auto mesh : meshes)
        {
            for (unsigned int i = 0; i < mesh->nodes.size(); i++)
            {
                auto node = mesh->nodes[i];
                node->force += g * node->mass;
            }
        }
    }
    for (auto mesh : meshes)
    {
        for (unsigned int i = 0; i < mesh->nodes.size(); i++)
        {
            auto node = mesh->nodes[i];
            node->anim = true;
        }

        _step(mesh);
    }
}

void AnimSystem::_addGravity(geometry::Nodes& nodes)
{
    if (gravity)
    {
        uint32_t size = nodes.size();
        geometry::Vec3 g = geometry::Vec3(0, -9.8f, 0);
        for (uint32_t i = 0; i < size; ++i)
            nodes[i]->force += g * nodes[i]->mass;
    }
}

void AnimSystem::_update(geometry::Nodes& nodes)
{
    uint32_t size = nodes.size();

    for (uint32_t i = 0; i < size; ++i)
    {
        auto node = nodes[i];
        if (!node->fix && !node->isSoma)
        {
            geometry::Vec3 a = node->force / node->mass;
            geometry::Vec3 v = node->velocity + a * _dt;
            geometry::Vec3 x = node->position + v * _dt;
            node->position = x;
            if (inertia) node->velocity = v;
        }
    }
}

void AnimSystem::_updateIfCollide(geometry::Nodes& nodes)
{
    for (uint32_t i = 0; i < nodes.size(); ++i)
    {
        auto node = nodes[i];
        if (!node->fix && !node->isSoma && node->collide)
        {
            geometry::Vec3 a = node->force / node->mass;
            geometry::Vec3 v = node->velocity + a * _dt;
            geometry::Vec3 x = node->position + v * _dt;
            node->position = x;
            if (inertia) node->velocity = v;
        }
    }
}

void AnimSystem::preprocessMesh(geometry::Meshes meshes)
{
    for (auto mesh : meshes)
    {
        preprocessMesh(mesh);
    }
}

}  // namespace anim
}  // namespace phyanim
