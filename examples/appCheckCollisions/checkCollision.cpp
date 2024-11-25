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

#include <phyanim/Phyanim.h>

#include <chrono>
#include <iomanip>
#include <iostream>

using namespace phyanim;

geometry::Meshes loadMeshes(std::vector<std::string> files)
{
    geometry::Meshes meshes(files.size());
    float progress = 0.0;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\rLoading files " << progress << "%" << std::flush;
    auto startTime = std::chrono::steady_clock::now();

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < files.size(); ++i)
    {
        geometry::MeshPtr mesh = new geometry::Mesh(50.0, 1.0, 1.0, 0.3);
        mesh->load(files[i]);
        mesh->boundingBox =
            new geometry::HierarchicalAABB(mesh->surfaceTriangles);
        meshes[i] = mesh;
#pragma omp critical
        {
            progress += 100.0f / files.size();
            std::cout << "\rLoading files " << progress << "%" << std::flush;
        }
    }
    std::cout << std::endl;
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = endTime - startTime;
    std::cout << "Files loaded in: " << elapsedTime.count() << " seconds"
              << std::endl;
    return meshes;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage error:\nUse: " << argv[0] << " file_name file_name"
                  << std::endl;
        return 0;
    }

    std::vector<std::string> files;
    for (uint32_t i = 1; i < argc; ++i)
    {
        std::string option(argv[i]);
        if (option.find(".tet") != std::string::npos)
            files.push_back(option);
        else if (option.find(".off") != std::string::npos)
            files.push_back(option);
    }

    auto meshes = loadMeshes(files);

    auto aabbs = anim::CollisionDetection::collisionBoundingBoxes(meshes, 15);
    std::cout << "Number of collisions: " << aabbs.size() << std::endl;

    return 0;
}
