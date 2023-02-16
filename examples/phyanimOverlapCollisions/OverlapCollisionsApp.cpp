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
    double stiffness = 50.0;
    double damping = 1.0;
    double density = 1.0;
    double poissonRatio = 0.3;
    double initCollisionStiffness = 2.0;
    double collisionStiffnessMultiplier = 0.1;
    double dt = 0.01;
    double bbFactor = 15;
    bool stepByStep = true;

    std::string usage = std::string(
        "SYNOPSIS\n\t"
        " mesh_file[.tet|.node .ele] [-dt double] [-d double] "
        "[-ks double] [-kd double] [-kp double] "
        "[--help]"
        "\nThe following options are available:"
        "\n\t-dt double    Set time increment"
        "\n\t-d double     Set density"
        "\n\t-ks double    Set stiffness constant/Young modulus"
        "\n\t-kd double    Set damping constant"
        "\n\t-kp double    Set poisson ratio"
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
                bbFactor = std::stof(_args[i + 1]);
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
    phyanim::AnimSystem* animSys = new phyanim::ImplicitFEMSystem(dt);
    animSys->gravity = false;

    _loadMeshes(files, stiffness, density, damping, poissonRatio);

    std::vector<phyanim::HierarchicalAABBPtr> tetAABBs(_meshes.size());
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < _meshes.size(); ++i)
    {
        _setSurfaceNodes(_meshes[i]);
        tetAABBs[i] = new phyanim::HierarchicalAABB(_meshes[i]->tetrahedra);
    }

    auto startTime = std::chrono::steady_clock::now();
    if (stepByStep)
    {
        for (uint32_t i = 0; i < _aabbs.size(); ++i)
        {
            auto aabb = _aabbs[i];
            _collisionId = i;
            std::cout << "Collision id: " << _collisionId << std::endl;

            phyanim::Meshes slicedMeshes;
            phyanim::Meshes animMeshes;
            std::vector<Mesh*> renderMeshes;
            for (uint32_t j = 0; j < _meshes.size(); ++j)
            {
                auto slicedMesh = _sliceMesh(tetAABBs[j], *aabb, stiffness,
                                             density, damping, poissonRatio);
                if (slicedMesh)
                {
                    slicedMeshes.push_back(slicedMesh);
                    animMeshes.push_back(_meshes[j]);
                    renderMeshes.push_back(_scene->meshes[j]);
                }
            }

            _setCameraPos(*aabb);

            animSys->preprocessMesh(slicedMeshes);

            _setAnim(!stepByStep);

            double collisionStiffness = initCollisionStiffness;

            bool collision = phyanim::CollisionDetection::computeCollisions(
                slicedMeshes, collisionStiffness);
            for (uint32_t meshId = 0; meshId < animMeshes.size(); ++meshId)
            {
                auto animMesh = animMeshes[meshId];
                auto renderMesh = renderMeshes[meshId];
                setGeometry(renderMesh, animMesh->nodes);
                setColorByCollision(renderMesh, animMesh->nodes);
            }
            while (collision)
            {
                if (_getAnim())
                {
                    for (auto mesh : slicedMeshes) mesh->nodesForceZero();
                    collision = phyanim::CollisionDetection::computeCollisions(
                        slicedMeshes, collisionStiffness);
                    if (collision)
                    {
                        animSys->step(slicedMeshes);
                        for (auto mesh : slicedMeshes)
                            mesh->boundingBox->update();
                        phyanim::CollisionDetection::computeCollisions(
                            slicedMeshes, _limits);
                        collisionStiffness += initCollisionStiffness *
                                              collisionStiffnessMultiplier;
                    }
                    for (uint32_t meshId = 0; meshId < animMeshes.size();
                         ++meshId)
                    {
                        setColorByCollision(renderMeshes[meshId],
                                            animMeshes[meshId]->nodes);
                        setGeometry(renderMeshes[meshId],
                                    animMeshes[meshId]->nodes);
                    }
                }
            }
        }
    }
    //     else
    //     {
    //         double progress = 0.0;
    //         std::cout << "\rSolving collisions " << progress << "%"
    //         << std::flush; uint32_t id = 0;
    // #ifdef PHYANIM_USES_OPENMP
    // #pragma omp parallel for
    // #endif
    //         for (uint32_t i = 0; i < _aabbs.size(); ++i)
    //         {
    //             uint32_t cId = i;
    // #ifdef PHYANIM_USES_OPENMP
    // #pragma omp critical
    //             {
    //                 cId = id;
    //                 ++id;
    //             }
    // #endif
    //             auto aabb = _aabbs[cId];
    //
    //             phyanim::Meshes slicedMeshes;
    //             phyanim::Meshes completeMeshes;
    //             for (uint32_t j = 0; j < _meshes.size(); ++j)
    //             {
    //                 auto slicedMesh = _sliceMesh(tetAABBs[j], *aabb,
    //                 stiffness,
    //                                              density, damping,
    //                                              poissonRatio);
    //                 if (slicedMesh)
    //                 {
    //                     slicedMeshes.push_back(slicedMesh);
    //                     completeMeshes.push_back(_meshes[j]);
    //                 }
    //             }
    //
    //             animSys->preprocessMesh(slicedMeshes);
    //             double currentCollisionStiffness =
    //             initCollisionStiffness;
    //
    //             bool collision = true;
    //             while (collision)
    //             {
    //                 for (auto mesh : slicedMeshes)
    //                 mesh->nodesForceZero(); collision =
    //                 phyanim::CollisionDetection::computeCollisions(
    //                     slicedMeshes, currentCollisionStiffness,
    //                     true);
    //                 if (collision)
    //                 {
    //                     animSys->step(slicedMeshes);
    //                     for (auto mesh : slicedMeshes)
    //                     mesh->boundingBox->update();
    //                     phyanim::CollisionDetection::computeCollisions(slicedMeshes,
    //                                                                    _limits);
    //                     for (auto mesh : completeMeshes)
    //                         dynamic_cast<phyanim::DrawableMesh*>(mesh)
    //                             ->updatedPositions = true;
    //                     currentCollisionStiffness +=
    //                         initCollisionStiffness *
    //                         collisionStiffnessMultiplier;
    //                 }
    //                 for (auto mesh : completeMeshes)
    //                     dynamic_cast<phyanim::DrawableMesh*>(mesh)->updateColors();
    //             }
    // #pragma omp critical
    //             {
    //                 progress += 100.0f / _aabbs.size();
    //                 std::cout << "\rSolving collisions " << progress
    //                 << "%"
    //                           << std::flush;
    //             }
    //         }
    //         std::cout << std::endl;
    //     }

    _setCameraPos(_limits);
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    std::cout << "Overlap solved in: " << elapsedTime.count() << " seconds"
              << std::endl;

    // _writeMeshes(_meshes, files, "_no_overlap");
}
phyanim::MeshPtr OverlapCollisionsApp::_sliceMesh(
    phyanim::HierarchicalAABBPtr tetAABB,
    const phyanim::AxisAlignedBoundingBox& aabb,
    double stiffness,
    double density,
    double damping,
    double poissonRatio)
{
    phyanim::Mesh* sliceMesh = nullptr;
    auto tets = tetAABB->insidePrimitives(aabb);
    if (tets.size() > 0)
    {
        sliceMesh =
            new phyanim::Mesh(stiffness, density, damping, poissonRatio);
        sliceMesh->tetrahedra = tets;
        sliceMesh->tetsToNodes();
        sliceMesh->tetsToTriangles();
        sliceMesh->boundingBox =
            new phyanim::HierarchicalAABB(sliceMesh->surfaceTriangles);
        sliceMesh->compute();

        phyanim::UniqueTriangles uniqueTriangles;

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

void OverlapCollisionsApp::_setSurfaceNodes(phyanim::MeshPtr mesh)
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
        glfwGetCursorPos(window, &_mouseX, &_mouseY);

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
