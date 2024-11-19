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

#include <phyanim/Phyanim.h>

#include <chrono>
#include <iomanip>

using namespace phyanim;

float stiffness = 50.0;
float dt = 0.01;
float bbFactor = 1.5;
float initCollisionStiffness = 2.0;
float collisionStiffnessMultiplier = 0.1;

geometry::Meshes meshes;
std::vector<geometry::HierarchicalAABBPtr> tetAABBs;
anim::AnimSystem* animSys;

void setSurfaceNodes(geometry::MeshPtr mesh)
{
    for (auto triangle : mesh->surfaceTriangles)
    {
        for (auto node : triangle->nodes())
        {
            node->surface = true;
        }
    }
}

void loadMeshes(std::vector<std::string> files)
{
    meshes.resize(files.size());
    tetAABBs.resize(files.size());
    float progress = 0.0;
    std::cout << "\rLoading files " << progress << "%" << std::flush;
    auto startTime = std::chrono::steady_clock::now();

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < files.size(); ++i)
    {
        auto mesh = new geometry::Mesh(stiffness, 1.0, 1.0, 0.3);
        mesh->load(files[i]);
        mesh->boundingBox =
            new geometry::HierarchicalAABB(mesh->surfaceTriangles);
        meshes[i] = mesh;
        tetAABBs[i] = new geometry::HierarchicalAABB(mesh->tetrahedra);
        setSurfaceNodes(mesh);
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
}

void writeMeshes(geometry::Meshes meshes,
                 std::vector<std::string> files,
                 std::string extension)
{
    float progress = 0.0;
    std::cout << "\rSaving files " << progress << "%" << std::flush;
    auto startTime = std::chrono::steady_clock::now();

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < meshes.size(); ++i)
    {
        auto outFile = files[i];
        uint32_t pos = outFile.find_last_of('/');
        if (pos != std::string::npos) outFile = outFile.substr(pos + 1);
        pos = outFile.find(".tet");
        if (pos != std::string::npos) outFile = outFile.substr(0, pos);
        outFile += extension + ".tet";
        meshes[i]->write(outFile);
#pragma omp critical
        {
            progress += 100.0f / meshes.size();
            std::cout << "\rSaving files " << progress << "%" << std::flush;
        }
    }
    std::cout << std::endl;
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = endTime - startTime;
    std::cout << "Files saved in: " << elapsedTime.count() << " seconds"
              << std::endl;
}

geometry::MeshPtr sliceMesh(geometry::HierarchicalAABBPtr tetAABB,
                            const geometry::AxisAlignedBoundingBox& aabb)
{
    geometry::MeshPtr sliceMesh = nullptr;
    auto tets = tetAABB->insidePrimitives(aabb);
    if (tets.size() > 0)
    {
        sliceMesh = new geometry::Mesh(stiffness, 1.0, 1.0, 0.3);
        sliceMesh->tetrahedra = tets;
        sliceMesh->tetsToNodes();
        sliceMesh->tetsToTriangles();
        sliceMesh->boundingBox =
            new geometry::HierarchicalAABB(sliceMesh->surfaceTriangles);
        sliceMesh->compute();

        geometry::UniqueTriangles uniqueTriangles;

        for (auto triangle : sliceMesh->surfaceTriangles)
        {
            bool surface = true;
            for (auto node : triangle->nodes())
            {
                surface &= node->surface;
            }
            if (!surface)
            {
                for (auto node : triangle->nodes())
                {
                    node->fix = true;
                }
            }
        }
    }
    return sliceMesh;
}

void sortAABBs(geometry::AxisAlignedBoundingBoxes& aabbs)
{
    auto cmp = [](geometry::AxisAlignedBoundingBoxPtr a,
                  geometry::AxisAlignedBoundingBoxPtr b) {
        return a->radius() > b->radius();
    };
    std::sort(aabbs.begin(), aabbs.end(), cmp);
}

void resolveCollision(geometry::AxisAlignedBoundingBoxPtr aabb)
{
    auto startTime = std::chrono::steady_clock::now();
    geometry::Meshes slicedMeshes;
    for (uint32_t j = 0; j < meshes.size(); ++j)
    {
        auto slicedMesh = sliceMesh(tetAABBs[j], *aabb);
        if (slicedMesh)
        {
            slicedMeshes.push_back(slicedMesh);
        }
    }
    animSys->preprocessMesh(slicedMeshes);
    float collisionStiffness = initCollisionStiffness;

    bool collision = true;
    while (collision)
    {
        for (auto mesh : slicedMeshes) mesh->nodesForceZero();
        collision = anim::CollisionDetection::computeCollisions(
            slicedMeshes, collisionStiffness);
        if (collision)
        {
            animSys->step(slicedMeshes);
            for (auto mesh : slicedMeshes) mesh->boundingBox->update();
            collisionStiffness +=
                initCollisionStiffness * collisionStiffnessMultiplier;
        }
    }

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = endTime - startTime;
#pragma omp critical
    {
        std::cout << "Collision with radius: " << aabb->radius()
                  << "\tsolved in: " << elapsedTime.count() << " seconds"
                  << std::endl;
    }
}

void resolveCollisions(geometry::AxisAlignedBoundingBoxes& aabbs)
{
    float progress = 0.0;
    // std::cout << "\rSolving collisions " << progress << "%" << std::flush;
    auto startTime = std::chrono::steady_clock::now();

    uint32_t id = 0;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < aabbs.size(); ++i)
    {
        uint32_t cId = i;

#ifdef PHYANIM_USES_OPENMP
#pragma omp critical
        {
            cId = id;
            ++id;
        }
#endif
        auto aabb = aabbs[cId];
        resolveCollision(aabb);
#pragma omp critical
        {
            progress += 100.0f / aabbs.size();
            // std::cout << "\rSolving collisions " << progress << "%"
            //           << std::endl;
        }
    }

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = endTime - startTime;
    std::cout << "Overlap solved in: " << elapsedTime.count() << " seconds"
              << std::endl;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> files;
    for (uint32_t i = 1; i < argc; ++i)
    {
        std::string option(argv[i]);

        if (option.compare("-dt") == 0)
        {
            ++i;
            dt = std::atof(argv[i]);
        }
        else if (option.compare("-k") == 0)
        {
            ++i;
            stiffness = std::atof(argv[i]);
        }
        else if (option.find(".tet") != std::string::npos)
            files.push_back(option);
    }

    animSys = new anim::ImplicitFEMSystem(dt);
    animSys->gravity = false;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Overlap run with dt: " << dt << " stiffness: " << stiffness
              << std::endl;

    // Loading files
    loadMeshes(files);

    // Collisions resolution
    auto aabbs =
        anim::CollisionDetection::collisionBoundingBoxes(meshes, bbFactor);
    std::cout << "Number of collisions: " << aabbs.size() << std::endl;
    sortAABBs(aabbs);
    resolveCollisions(aabbs);

    // Writing files
    writeMeshes(meshes, files, "_no_overlap");
}