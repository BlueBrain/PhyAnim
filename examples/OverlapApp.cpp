#include "OverlapApp.h"

#include <ExplicitFEMSystem.h>
#include <ImplicitFEMSystem.h>

#include <iostream>

namespace examples
{
OverlapApp::OverlapApp()
    : GLFWApp()
    , _anim(true)
    , _mesh(nullptr)
    , _stiffness(1000.0)
    , _damping(1.0)
    , _density(1.0)
    , _poissonRatio(0.49)
    , _collisionStiffness(100.0)
    , _dt(0.01)
{
}

void OverlapApp::init(int argc, char** argv)
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

    _scene = new Scene();
    _animSys = new phyanim::ImplicitFEMSystem(_dt);
    _animSys->gravity = false;
    _collisionSys = new phyanim::CollisionDetection(_collisionStiffness);

    phyanim::AABB limits;

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
            outFile = file.substr(0, extPos) + ".off";
        }
        else if ((extPos = file.find(".tet")) != std::string::npos)
        {
            mesh = new phyanim::DrawableMesh(_stiffness, _density, _damping,
                                             _poissonRatio);
            mesh->load(file);
            outFile = file.substr(0, extPos) + "_sol.tet";
        }

        if (mesh)
        {
            _files.push_back(outFile);
            _meshes.push_back(mesh);
            mesh->upload();
            limits.update(mesh->aabb->root->aabb);
        }
    }

    // phyanim::Vec3 halfSides = limits.upperLimit - limits.center();
    // limits.lowerLimit -= halfSides;
    // limits.upperLimit += halfSides;
    _setCameraPos(limits);
    _collisionSys->aabb = limits;
    _startTime = std::chrono::steady_clock::now();
}

void OverlapApp::loop()
{
    while (!glfwWindowShouldClose(_window))
    {
        if (_anim)
        {
            if (_mesh)
            {
                _mesh->nodesForceZero();
                if (_collisionSys->update())
                {
                    _animSys->step();
                    _mesh->aabb->update();
                    _collisionSys->checkLimitsCollision();
                    auto drawMesh = dynamic_cast<phyanim::DrawableMesh*>(_mesh);
                    drawMesh->uploadNodes();
                }
                else
                {
                    double mean, max, min, rms;
                    _mesh->positionDifference(mean, max, min, rms);
                    std::cout
                        << "\t* Original volume: " << _mesh->initVolume
                        << ". Volume difference: "
                        << (_mesh->initVolume / _mesh->volume() - 1.0) * 100.0
                        << "%" << std::endl;
                    std::cout << "\t* Vertices distance mean: " << mean
                              << " max: " << max << " min: " << min
                              << " rms: " << rms << std::endl;

                    _staticMeshes.push_back(_mesh);
                    _mesh->write(_file);
                    std::cout << "Writed file " << _file << std::endl;
                    _mesh = nullptr;
                }
            }
            else
            {
                if (!_meshes.empty())
                {
                    _mesh = _meshes[0];
                    _file = _files[0];
                    _meshes.erase(_meshes.begin());
                    _files.erase(_files.begin());
                    auto drawMesh = dynamic_cast<phyanim::DrawableMesh*>(_mesh);
                    _scene->addMesh(drawMesh);

                    phyanim::Meshes dynamicMeshes;
                    dynamicMeshes.push_back(_mesh);

                    _collisionSys->dynamicMeshes(dynamicMeshes);
                    _collisionSys->staticMeshes(_staticMeshes);

                    _animSys->clear();
                    _animSys->addMesh(_mesh);
                    _anim = false;
                }
                else
                {
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
}

void OverlapApp::_keyCallback(GLFWwindow* window,
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
