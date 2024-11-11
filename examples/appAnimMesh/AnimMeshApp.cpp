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

#include "AnimMeshApp.h"

#include <iostream>

using namespace phyanim;

int main(int argc, char* argv[])
{
    examples::AnimMeshApp app(argc, argv);
    app.run();

    return 0;
}

namespace examples
{
void AnimMeshApp::_actionLoop()
{
    float stiffness = 1000.0;
    float poisson = 0.3;
    float collisionStiffness = 200.0;
    float dt = 0.01;
    float damping = 1.0;
    float density = 1.0;

    anim::AnimSystem* animSys = new anim::ImplicitFEMSystem(dt);

    geometry::AxisAlignedBoundingBox limits;

    for (uint64_t i = 0; i < _args.size(); ++i)
    {
        auto mesh = new geometry::Mesh(stiffness, density, damping, poisson);
        std::string file = _args[i];

        if (file.find(".node") != std::string::npos)
        {
            ++i;
            std::string file1 = _args[i];
            mesh->load(file, file1);
        }
        else if (file.find(".tet") != std::string::npos)
            mesh->load(file);
        else
            std::cerr << "Error: Unknown format file." << std::endl;

        mesh->compute();

        _meshes.push_back(mesh);
        mesh->boundingBox =
            new geometry::HierarchicalAABB(mesh->surfaceTriangles);
        limits.unite(*mesh->boundingBox);
        _setCameraPos(limits);

        _scene->meshes.push_back(
            graphics::generateMesh(mesh->nodes, mesh->surfaceTriangles));
    }

    animSys->preprocessMesh(_meshes);

    geometry::Vec3 halfSides = limits.upperLimit() - limits.center();
    limits.lowerLimit(limits.lowerLimit() - halfSides);
    limits.upperLimit(limits.upperLimit() + halfSides);

    while (true)
    {
        if (_getAnim())
        {
            for (auto mesh : _meshes) mesh->nodesForceZero();

            anim::CollisionDetection::computeCollisions(_meshes,
                                                        collisionStiffness);

            animSys->step(_meshes);

            for (auto mesh : _meshes) mesh->boundingBox->update();

            anim::CollisionDetection::computeCollisions(_meshes, limits);

            for (uint32_t i = 0; i < _meshes.size(); ++i)
            {
                setGeometry(_scene->meshes[i], _meshes[i]->nodes);
                setColorByCollision(_scene->meshes[i], _meshes[i]->nodes);
            }
        }
    }
}

}  // namespace examples
