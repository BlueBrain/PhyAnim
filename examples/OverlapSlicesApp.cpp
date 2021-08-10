#include "OverlapSlicesApp.h"

#include <ImplicitFEMSystem.h>

#include <iostream>

namespace examples
{
OverlapSlicesApp::OverlapSlicesApp()
    : GLFWApp()
    , _anim(true)
    , _stiffness(100.0)
    , _damping(1.0)
    , _density(1.0)
    , _poissonRatio(0.3)
    , _initCollisionStiffness(2.0)
    , _collisionStiffness(2.0)
    , _collisionStiffnessMultiplier(0.1)
    , _dt(0.01)
    , _stepByStep(true)
    , _bbFactor(20)
{
}

void OverlapSlicesApp::init(int argc, char** argv)
{
    std::string usage =
        std::string("SYNOPSIS\n\t") + std::string(argv[0]) +
        std::string(
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
    if (argc < 2)
    {
        std::cerr << "Usage error:\n" << usage << std::endl;
        exit(-1);
    }

    std::vector<std::string> files;
    for (int i = 1; i < argc; ++i)
    {
        std::string option(argv[i]);
        try
        {
            if (option.compare("-dt") == 0)
            {
                _dt = std::atof(argv[i + 1]);
                ++i;
            }
            if (option.compare("-bb") == 0)
            {
                _bbFactor = std::atof(argv[i + 1]);
                ++i;
            }
            else if (option.compare("-ks") == 0)
            {
                _stiffness = std::atof(argv[i + 1]);
                ++i;
            }
            else if (option.compare("-d") == 0)
            {
                _density = std::atof(argv[i + 1]);
                ++i;
            }
            else if (option.compare("-kd") == 0)
            {
                _damping = std::atof(argv[i + 1]);
                ++i;
            }
            else if (option.compare("-kp") == 0)
            {
                _poissonRatio = std::atof(argv[i + 1]);
                ++i;
            }
            else if (option.compare("-kc") == 0)
            {
                _collisionStiffness = std::atof(argv[i + 1]);
                ++i;
            }
            else if (option.compare("-kcm") == 0)
            {
                _collisionStiffnessMultiplier = std::atof(argv[i + 1]);
                ++i;
            }
            else if (option.compare("-cont") == 0)
            {
                _stepByStep = false;
            }
            else if (option.compare("-cellSize") == 0)
            {
                _cellSize = std::atoi(argv[i + 1]);
                ++i;
            }
            else if (option.compare("--help") == 0)
            {
                std::cout << usage << std::endl;
                exit(0);
            }
            else
            {
                files.push_back(std::string(argv[i]));
            }
        }
        catch (...)
        {
            std::cerr << "Usage error:\n" << usage << std::endl;
            exit(-1);
        }
    }

    _initCollisionStiffness = _collisionStiffness;

    _scene = new Scene();
    _animSys = new phyanim::ImplicitFEMSystem(_dt);
    _animSys->gravity = false;

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
            mesh = new phyanim::DrawableMesh(_stiffness, _density, _damping,
                                             _poissonRatio);
            mesh->load(file, file1);
        }
        else if ((extPos = file.find(".tet")) != std::string::npos)
        {
            mesh = new phyanim::DrawableMesh(_stiffness, _density, _damping,
                                             _poissonRatio);
            mesh->load(file);
        }

        if (mesh)
        {
            std::string outFile = file.substr(0, extPos) + "_sol.tet";
            _inFiles.push_back(file);
            _outFiles.push_back(outFile);
            _meshes.push_back(mesh);
            mesh->upload();
            mesh->compute();
            _setSurfaceNodes(mesh);
            mesh->boundingBox =
                new phyanim::HierarchicalAABB(mesh->surfaceTriangles);
            _limits.unite(*mesh->boundingBox);
            _scene->addMesh(mesh);
        }
    }

    _setCameraPos(_limits);

    _startTime = std::chrono::steady_clock::now();

    _aabbs =
        phyanim::CollisionDetection::collisionBoundingBoxes(_meshes, _bbFactor);
    if (_stepByStep)
    {
        _anim = false;
    }
}

void OverlapSlicesApp::loop()
{
    while (!glfwWindowShouldClose(_window))
    {
        if (_anim)
        {
            if (_collisionMeshes.size() > 0)
            {
                for (auto mesh : _collisionMeshes)
                {
                    mesh->nodesForceZero();
                }
                if (phyanim::CollisionDetection::computeCollisions(
                        _collisionMeshes, _collisionStiffness, true))
                {
                    _animSys->step(_collisionMeshes);
                    for (auto mesh : _collisionMeshes)
                    {
                        mesh->boundingBox->update();
                    }
                    phyanim::CollisionDetection::computeCollisions(
                        _collisionMeshes, _limits);
                    for (auto mesh : _meshes)
                    {
                        auto drawMesh =
                            dynamic_cast<phyanim::DrawableMesh*>(mesh);
                        drawMesh->uploadPositions();
                    }
                    _collisionStiffness +=
                        _initCollisionStiffness * _collisionStiffnessMultiplier;
                }
                else
                {
                    for (auto mesh : _meshes)
                    {
                        auto drawMesh =
                            dynamic_cast<phyanim::DrawableMesh*>(mesh);
                    }
                    _collisionMeshes.clear();
                }
                _scene->updateColors();
            }
            else
            {
                if (!_aabbs.empty())
                {
                    auto aabb = _aabbs[0];
                    _setCameraPos(*aabb);
                    _aabbs.erase(_aabbs.begin());
                    for (auto mesh : _meshes)
                    {
                        auto collisionMesh = _sliceMesh(mesh, *aabb);
                        if (collisionMesh)
                        {
                            _collisionMeshes.push_back(collisionMesh);
                        }
                    }
                    _animSys->preprocessMesh(_collisionMeshes);
                    _collisionStiffness = _initCollisionStiffness;
                    if (_stepByStep)
                    {
                        _anim = false;
                    }
                }
                else
                {
                    double mean, max, min, rms;
                    for (uint64_t i = 0; i < _meshes.size(); ++i)
                    {
                        auto mesh = _meshes[i];
                        mesh->positionDifference(mean, max, min, rms);
                        std::cout
                            << "Mesh " << _inFiles[i] << ":\n"
                            << "\t* Original volume: " << mesh->initVolume
                            << ". Volume difference: "
                            << (mesh->initVolume / mesh->volume() - 1.0) * 100.0
                            << "%" << std::endl;
                        std::cout << "\t* Vertices distance mean: " << mean
                                  << " max: " << max << " min: " << min
                                  << " rms: " << rms << std::endl;

                        mesh->write(_outFiles[i]);
                        std::cout << "\t* Saved file " << _outFiles[i]
                                  << std::endl;
                    }
                    auto endTime = std::chrono::steady_clock::now();
                    std::chrono::duration<double> elapsedTime =
                        endTime - _startTime;
                    std::cout << "Overlap solved in: " << elapsedTime.count()
                              << "seconds" << std::endl;
                    _anim = false;
                }
            }
        }
        _scene->render();
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    glfwTerminate();
}  // namespace examples

phyanim::MeshPtr OverlapSlicesApp::_sliceMesh(
    phyanim::MeshPtr mesh,
    const phyanim::AxisAlignedBoundingBox& aabb)
{
    phyanim::DrawableMesh* sliceMesh = nullptr;
    auto tetsAABB = new phyanim::HierarchicalAABB(mesh->tetrahedra);
    auto tets = tetsAABB->insidePrimitives(aabb);

    delete tetsAABB;

    if (tets.size() > 0)
    {
        sliceMesh = new phyanim::DrawableMesh(_stiffness, _density, _damping,
                                              _poissonRatio);
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

        // for (auto triangle : mesh->surfaceTriangles)
        // {
        //     uniqueTriangles.insert(
        //         dynamic_cast<phyanim::TrianglePtr>(triangle));
        // }
        // for (auto triangle : sliceMesh->surfaceTriangles)
        // {
        //     if (uniqueTriangles.find(dynamic_cast<phyanim::TrianglePtr>(
        //             triangle)) != uniqueTriangles.end())
        //     {
        //         for (auto node : triangle->nodes())
        //         {
        //             node->fix = true;
        //         }
        //     }
        // }
    }
    return sliceMesh;
}

void OverlapSlicesApp::_setSurfaceNodes(phyanim::MeshPtr mesh)
{
    for (auto triangle : mesh->surfaceTriangles)
    {
        for (auto node : triangle->nodes())
        {
            node->surface = true;
        }
    }
}

void OverlapSlicesApp::_keyCallback(GLFWwindow* window,
                                    int key,
                                    int scancode,
                                    int action,
                                    int mods)
{
    GLFWApp::_keyCallback(window, key, scancode, action, mods);
    if (_scene)
    {
        if (action == GLFW_RELEASE)
        {
            switch (key)
            {
            case GLFW_KEY_SPACE:
                _anim = !_anim;
                if (_anim)
                    std::cout << "Release" << std::endl;
                else
                    std::cout << "Pause" << std::endl;
                break;
            }
        }
    }
}

}  // namespace examples
