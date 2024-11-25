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

#include "OverlapCollisionsApp.h"

#include <iomanip>
#include <iostream>

int main(int argc, char* argv[])
{
    examples::OverlapCollisionsApp app(argc, argv);
    app.run();

    return 0;
}

namespace examples
{
OverlapCollisionsApp::OverlapCollisionsApp(int argc, char** argv)
    : GLFWApp(argc, argv)
{
}

void OverlapCollisionsApp::_actionLoop()
{
    float stiffness = 50.0;
    float damping = 1.0;
    float density = 1.0;
    float poissonRatio = 0.3;
    float initCollisionStiffness = 2.0;
    float collisionStiffnessMultiplier = 0.1;
    float dt = 0.01;
    _bbFactor = 1.5;
    bool stepByStep = true;

    std::string usage = std::string(
        "SYNOPSIS\n\t"
        " mesh_file[.tet|.node .ele] [-dt float] [-d float] "
        "[-ks float] [-kd float] [-kp float] "
        "[--help]"
        "\nThe following options are available:"
        "\n\t-dt float    Set time increment"
        "\n\t-d float     Set density"
        "\n\t-ks float    Set stiffness constant/Young modulus"
        "\n\t-kd float    Set damping constant"
        "\n\t-kp float    Set poisson ratio"
        "\n\t--help        Print help message");
    if (_args.size() == 0)
    {
        std::cerr << "Usage error:\n" << usage << std::endl;
        exit(-1);
    }

    std::vector<std::string> files;
    for (int i = 0; i < _args.size(); ++i)
    {
        std::string option(_args[i]);
        try
        {
            if (option.compare("-dt") == 0)
            {
                dt = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-bb") == 0)
            {
                _bbFactor = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-k") == 0)
            {
                stiffness = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-d") == 0)
            {
                density = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-kd") == 0)
            {
                damping = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-kp") == 0)
            {
                poissonRatio = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-kc") == 0)
            {
                initCollisionStiffness = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-kcm") == 0)
            {
                collisionStiffnessMultiplier = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-cont") == 0)
            {
                stepByStep = false;
            }
            else if (option.compare("--help") == 0)
            {
                std::cout << usage << std::endl;
                exit(0);
            }
            else if (_args[i].find(".tet") != std::string::npos)
                files.push_back(_args[i]);
            else
                std::cerr << "Unknown file format: " << _args[i] << std::endl;
        }
        catch (...)
        {
            std::cerr << "Usage error:\n" << usage << std::endl;
            exit(-1);
        }
    }
    std::cout << "Overlap run with dt: " << dt << " stiffness: " << stiffness
              << std::endl;
    anim::AnimSystem* animSys = new anim::ImplicitFEMSystem(dt);
    animSys->gravity = false;

    _loadMeshes(files, stiffness, density, damping, poissonRatio);

    std::vector<geometry::HierarchicalAABBPtr> tetAABBs(_meshes.size());
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < _meshes.size(); ++i)
    {
        _setSurfaceNodes(_meshes[i]);
        tetAABBs[i] = new geometry::HierarchicalAABB(_meshes[i]->tetrahedra);
    }

    auto startTime = std::chrono::steady_clock::now();

    for (uint32_t i = 0; i < _aabbs.size(); ++i)
    {
        auto aabb = _aabbs[i];
        _collisionId = i;
        std::cout << "Collision id: " << _collisionId << std::endl;

        geometry::Meshes slicedMeshes;
        geometry::Meshes animMeshes;
        std::vector<graphics::Mesh*> renderMeshes;
        for (uint32_t j = 0; j < _meshes.size(); ++j)
        {
            auto slicedMesh = _sliceMesh(tetAABBs[j], *aabb, stiffness, density,
                                         damping, poissonRatio);
            if (slicedMesh)
            {
                slicedMeshes.push_back(slicedMesh);
                animMeshes.push_back(_meshes[j]);
                renderMeshes.push_back(_scene->meshes[j]);
                for (auto node : slicedMesh->nodes) node->collide = true;
            }
        }

        phyanim::geometry::AxisAlignedBoundingBox limits(*aabb);
        limits.resize(2.0f);
        _setCameraPos(limits);

        animSys->preprocessMesh(slicedMeshes);

        _setAnim(!stepByStep);

        float collisionStiffness = initCollisionStiffness;

        bool collision = anim::CollisionDetection::computeCollisions(
            slicedMeshes, collisionStiffness);
        for (uint32_t meshId = 0; meshId < animMeshes.size(); ++meshId)
        {
            auto animMesh = animMeshes[meshId];
            auto renderMesh = renderMeshes[meshId];
            graphics::setGeometry(renderMesh, animMesh->nodes);
            graphics::setColorByCollision(renderMesh, animMesh->nodes);
        }
        while (collision)
        {
            if (_getAnim())
            {
                for (auto mesh : slicedMeshes) mesh->nodesForceZero();
                collision = anim::CollisionDetection::computeCollisions(
                    slicedMeshes, collisionStiffness);
                if (collision)
                {
                    animSys->step(slicedMeshes);
                    for (auto mesh : slicedMeshes) mesh->boundingBox->update();
                    anim::CollisionDetection::computeCollisions(slicedMeshes,
                                                                *aabb);
                    collisionStiffness +=
                        initCollisionStiffness * collisionStiffnessMultiplier;
                }
                for (uint32_t meshId = 0; meshId < animMeshes.size(); ++meshId)
                {
                    // graphics::setColorByCollision(
                    //     renderMeshes[meshId],
                    // animMeshes[meshId]->nodes);
                    graphics::setGeometry(renderMeshes[meshId],
                                          animMeshes[meshId]->nodes);
                }
            }
        }
    }

    _setCameraPos(_limits);
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = endTime - startTime;
    std::cout << "Overlap solved in: " << elapsedTime.count() << " seconds"
              << std::endl;

    // _writeMeshes(_meshes, files, "_no_overlap");
}
geometry::MeshPtr OverlapCollisionsApp::_sliceMesh(
    geometry::HierarchicalAABBPtr tetAABB,
    const geometry::AxisAlignedBoundingBox& aabb,
    float stiffness,
    float density,
    float damping,
    float poissonRatio)
{
    geometry::Mesh* sliceMesh = nullptr;
    auto tets = tetAABB->insidePrimitives(aabb);
    if (tets.size() > 0)
    {
        sliceMesh =
            new geometry::Mesh(stiffness, density, damping, poissonRatio);
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

void OverlapCollisionsApp::_setSurfaceNodes(geometry::MeshPtr mesh)
{
    for (auto triangle : mesh->surfaceTriangles)
    {
        for (auto node : triangle->nodes())
        {
            node->surface = true;
        }
    }
}

void OverlapCollisionsApp::_mouseButtonCallback(GLFWwindow* window,
                                                int button,
                                                int action,
                                                int mods)
{
    if (_scene)
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        _mouseX = mouseX;
        _mouseY = mouseY;

        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
            {
                _leftButtonPressed = true;
            }
            else if (action == GLFW_RELEASE)
            {
                _leftButtonPressed = false;
            }
        }
        if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            if (action == GLFW_PRESS)
            {
                _middleButtonPressed = true;
            }
            else if (action == GLFW_RELEASE)
            {
                _middleButtonPressed = false;
            }
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            if (action == GLFW_PRESS)
            {
                _rightButtonPressed = true;
            }
            else if (action == GLFW_RELEASE)
            {
                _rightButtonPressed = false;
            }
        }
    }
}

}  // namespace examples
