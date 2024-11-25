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
