#include "OverlapCollisionsApp.h"

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
    double stiffness = 100.0;
    double damping = 1.0;
    double density = 1.0;
    double poissonRatio = 0.3;
    double initCollisionStiffness = 2.0;
    double collisionStiffness = 2.0;
    double collisionStiffnessMultiplier = 0.1;
    double dt = 0.01;
    double bbFactor = 10;
    bool stepByStep = true;

    std::chrono::time_point<std::chrono::steady_clock> startTime;

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
            if (option.compare("-bb") == 0)
            {
                bbFactor = std::stof(_args[i + 1]);
                ++i;
            }
            else if (option.compare("-ks") == 0)
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
                collisionStiffness = std::stof(_args[i + 1]);
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
            else
            {
                files.push_back(std::string(_args[i]));
            }
        }
        catch (...)
        {
            std::cerr << "Usage error:\n" << usage << std::endl;
            exit(-1);
        }
    }

    initCollisionStiffness = collisionStiffness;

    phyanim::AnimSystem* animSys = new phyanim::ImplicitFEMSystem(dt);
    animSys->gravity = false;

    phyanim::Meshes meshes;
    std::vector<std::string> outFiles;

    phyanim::AxisAlignedBoundingBox limits;

    for (uint32_t i = 0; i < files.size(); ++i)
    {
        phyanim::DrawableMesh* mesh = nullptr;
        std::string file = files[i];

        std::string outFile;
        size_t extPos = 0;
        if ((extPos = file.find(".node")) != std::string::npos)
        {
            std::string file1 = files[i + 1];
            ++i;
            mesh = new phyanim::DrawableMesh(stiffness, density, damping,
                                             poissonRatio);
            mesh->load(file, file1);
        }
        else if ((extPos = file.find(".tet")) != std::string::npos)
        {
            mesh = new phyanim::DrawableMesh(stiffness, density, damping,
                                             poissonRatio);
            mesh->load(file);
        }

        if (mesh)
        {
            std::string outFile = file.substr(0, extPos) + "_sol.tet";
            outFiles.push_back(outFile);
            meshes.push_back(mesh);
            // mesh->compute();
            _setSurfaceNodes(mesh);
            mesh->boundingBox =
                new phyanim::HierarchicalAABB(mesh->surfaceTriangles);
            limits.unite(*mesh->boundingBox);
            _scene->addMesh(mesh);
        }
    }

    _setCameraPos(limits);

    startTime = std::chrono::steady_clock::now();

    phyanim::AxisAlignedBoundingBoxes aabbs =
        phyanim::CollisionDetection::collisionBoundingBoxes(meshes, bbFactor);

    for (auto aabb : aabbs)
    {
        _setCameraPos(*aabb);

        phyanim::Meshes slicedMeshes;
        phyanim::Meshes completeMeshes;
        for (auto mesh : meshes)
        {
            auto slicedMesh = _sliceMesh(mesh, *aabb, stiffness, density,
                                         damping, poissonRatio);
            if (slicedMesh)
            {
                slicedMeshes.push_back(slicedMesh);
                completeMeshes.push_back(mesh);
            }
        }

        animSys->preprocessMesh(slicedMeshes);

        _setAnim(!stepByStep);

        collisionStiffness = initCollisionStiffness;

        bool collision = true;
        while (collision)
        {
            if (_getAnim())
            {
                std::cout << "anim" << std::endl;
                for (auto mesh : slicedMeshes) mesh->nodesForceZero();
                collision = phyanim::CollisionDetection::computeCollisions(
                    slicedMeshes, collisionStiffness, true);
                if (collision)
                {
                    animSys->step(slicedMeshes);
                    for (auto mesh : slicedMeshes) mesh->boundingBox->update();
                    phyanim::CollisionDetection::computeCollisions(slicedMeshes,
                                                                   limits);
                    for (auto mesh : completeMeshes)
                        dynamic_cast<phyanim::DrawableMesh*>(mesh)
                            ->updatedPositions = true;
                    collisionStiffness +=
                        initCollisionStiffness * collisionStiffnessMultiplier;

                    for (auto mesh : completeMeshes)
                        dynamic_cast<phyanim::DrawableMesh*>(mesh)
                            ->updateColors();
                }
                else
                {
                    _setAnim(!stepByStep);
                }
            }
        }
    }
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    std::cout << "Overlap solved in: " << elapsedTime.count() << " seconds"
              << std::endl;
}

// void OverlapCollisionsApp::loop()
// {
//     while (!glfwWindowShouldClose(_window))
//     {
//         if (_anim)
//         {
//             if (_collisionMeshes.size() > 0)
//             {
//                 for (auto mesh : _collisionMeshes)
//                 {
//                     mesh->nodesForceZero();
//                 }
//                 if (phyanim::CollisionDetection::computeCollisions(
//                         _collisionMeshes, _collisionStiffness, true))
//                 {
//                     _animSys->step(_collisionMeshes);
//                     for (auto mesh : _collisionMeshes)
//                     {
//                         mesh->boundingBox->update();
//                     }
//                     phyanim::CollisionDetection::computeCollisions(
//                         _collisionMeshes, _limits);
//                     for (auto mesh : _meshes)
//                     {
//                         auto drawMesh =
//                             dynamic_cast<phyanim::DrawableMesh*>(mesh);
//                         drawMesh->uploadPositions();
//                     }
//                     _collisionStiffness +=
//                         _initCollisionStiffness *
//                         _collisionStiffnessMultiplier;
//                 }
//                 else
//                 {
//                     for (auto mesh : _meshes)
//                     {
//                         auto drawMesh =
//                             dynamic_cast<phyanim::DrawableMesh*>(mesh);
//                     }
//                     _collisionMeshes.clear();
//                 }
//                 _scene->updateColors();
//             }
//             else
//             {
//                 if (!_aabbs.empty())
//                 {
//                     auto aabb = _aabbs[0];
//                     _setCameraPos(*aabb);
//                     _aabbs.erase(_aabbs.begin());
//                     for (auto mesh : _meshes)
//                     {
//                         auto collisionMesh = _sliceMesh(mesh, *aabb);
//                         if (collisionMesh)
//                         {
//                             _collisionMeshes.push_back(collisionMesh);
//                         }
//                     }
//                     _animSys->preprocessMesh(_collisionMeshes);
//                     _collisionStiffness = _initCollisionStiffness;
//                     if (_stepByStep)
//                     {
//                         _anim = false;
//                     }
//                 }
//                 else
//                 {
//                     double mean, max, min, rms;
//                     for (uint64_t i = 0; i < _meshes.size(); ++i)
//                     {
//                         auto mesh = _meshes[i];
//                         mesh->positionDifference(mean, max, min, rms);
//                         std::cout
//                             << "Mesh " << _inFiles[i] << ":\n"
//                             << "\t* Original volume: " << mesh->initVolume
//                             << ". Volume difference: "
//                             << (mesh->initVolume / mesh->volume() - 1.0) *
//                             100.0
//                             << "%" << std::endl;
//                         std::cout << "\t* Vertices distance mean: " << mean
//                                   << " max: " << max << " min: " << min
//                                   << " rms: " << rms << std::endl;

//                         mesh->write(_outFiles[i]);
//                         std::cout << "\t* Saved file " << _outFiles[i]
//                                   << std::endl;
//                     }
//                     auto endTime = std::chrono::steady_clock::now();
//                     std::chrono::duration<double> elapsedTime =
//                         endTime - _startTime;
//                     std::cout << "Overlap solved in: " << elapsedTime.count()
//                               << "seconds" << std::endl;
//                     _anim = false;
//                 }
//             }
//         }
//         _scene->render();
//         glfwSwapBuffers(_window);
//         glfwPollEvents();
//     }
//     glfwTerminate();
// }

phyanim::MeshPtr OverlapCollisionsApp::_sliceMesh(
    phyanim::MeshPtr mesh,
    const phyanim::AxisAlignedBoundingBox& aabb,
    double stiffness,
    bool density,
    bool damping,
    bool poissonRatio)
{
    phyanim::DrawableMesh* sliceMesh = nullptr;
    auto tetsAABB = new phyanim::HierarchicalAABB(mesh->tetrahedra);
    auto tets = tetsAABB->insidePrimitives(aabb);

    delete tetsAABB;

    if (tets.size() > 0)
    {
        sliceMesh = new phyanim::DrawableMesh(stiffness, density, damping,
                                              poissonRatio);
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

}  // namespace examples
