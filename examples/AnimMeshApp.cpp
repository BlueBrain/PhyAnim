#include "AnimMeshApp.h"

#include <ExplicitFEMSystem.h>
#include <ExplicitMassSpringSystem.h>
#include <ImplicitFEMSystem.h>
#include <ImplicitMassSpringSystem.h>

#include <iostream>

namespace examples
{
AnimMeshApp::AnimMeshApp() : GLFWApp(), _anim(false) {}

void AnimMeshApp::init(int argc, char** argv)
{
    _scene = new Scene();

    std::string usage = std::string("Use ") + std::string(argv[0]) +
                        std::string(
                            " mesh_file[.tet|.node .ele] -ks [double]  -kp "
                            "[double]  -kc [double] -dt [double] -d [double]");
    if (argc < 2)
    {
        std::cerr << "Usage error: " << usage << std::endl;
        exit(-1);
    }

    double stiffness = 1000.0;
    double poisson = 0.499;
    double collisionStiffness = 100.0;
    double dt = 0.01;
    double damping = 1.0;
    double density = 1.0;
    SimSystem simSystem = exmass;

    std::vector<std::string> files;
    for (uint64_t i = 1; i < argc; ++i)
    {
        std::string option(argv[i]);
        if (option.compare("-ks") == 0)
        {
            ++i;
            stiffness = std::atof(argv[i]);
        }
        else if (option.compare("-kp") == 0)
        {
            ++i;
            poisson = std::atof(argv[i]);
        }
        else if (option.compare("-kc") == 0)
        {
            ++i;
            collisionStiffness = std::atof(argv[i]);
        }
        else if (option.compare("-dt") == 0)
        {
            ++i;
            dt = std::atof(argv[i]);
        }
        else if (option.compare("-d") == 0)
        {
            ++i;
            damping = std::atof(argv[i]);
        }
        else if (option.compare("-density") == 0)
        {
            ++i;
            density = std::atof(argv[i]);
        }
        else if (option.compare("-exmass") == 0)
        {
            simSystem = exmass;
        }
        else if (option.compare("-immass") == 0)
        {
            simSystem = immass;
        }
        else if (option.compare("-exfem") == 0)
        {
            simSystem = exfem;
        }
        else if (option.compare("-imfem") == 0)
        {
            simSystem = imfem;
        }
        else if (option.compare("-help") == 0)
        {
            std::cerr << usage << std::endl;
            exit(0);
        }
        else
        {
            files.push_back(option);
        }
    }

    bool createEdges = false;

    switch (simSystem)
    {
    case exmass:
        _animSys = new phyanim::ExplicitMassSpringSystem(dt);
        createEdges = true;
        break;
    case immass:
        _animSys = new phyanim::ImplicitMassSpringSystem(dt);
        createEdges = true;
        break;
    case exfem:
        _animSys = new phyanim::ExplicitFEMSystem(dt);
        break;
    case imfem:
        _animSys = new phyanim::ImplicitFEMSystem(dt);
        break;
    }
    _collisionSys = new phyanim::CollisionDetection(collisionStiffness);

    phyanim::AABB limits;
    for (uint64_t i = 0; i < files.size(); ++i)
    {
        auto mesh =
            new phyanim::DrawableMesh(stiffness, density, damping, poisson);
        std::string file = files[i];

        if (file.find(".node") != std::string::npos)
        {
            ++i;
            std::string file1 = files[i];
            mesh->load(file, file1, createEdges);
        }
        else if (file.find(".tet") != std::string::npos)
        {
            mesh->load(file, createEdges);
        }
        else
        {
            std::cerr << "Error: Unknown format file." << std::endl;
            exit(-1);
        }

        _meshes.push_back(mesh);
        mesh->upload();
        limits.update(mesh->aabb->root->aabb);
        _scene->addMesh(mesh);
        _animSys->addMesh(mesh);
    }
    _collisionSys->dynamicMeshes(_meshes);

    phyanim::Vec3 halfSides = limits.upperLimit - limits.center();
    limits.lowerLimit -= halfSides;
    limits.upperLimit += halfSides;
    _setCameraPos(limits);
    _collisionSys->aabb = limits;
}

void AnimMeshApp::loop()
{
    while (!glfwWindowShouldClose(_window))
    {
        if (_anim)
        {
            for (auto mesh : _meshes)
            {
                mesh->nodesForceZero();
            }
            _collisionSys->update();
            _animSys->step();
            for (auto mesh : _meshes)
            {
                mesh->aabb->update();
            }
            _collisionSys->checkLimitsCollision();
            for (auto mesh : _meshes)
            {
                auto drawMesh = dynamic_cast<phyanim::DrawableMesh*>(mesh);
                drawMesh->uploadNodes();
            }
        }
        _scene->render();
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    glfwTerminate();
}

void AnimMeshApp::_keyCallback(GLFWwindow* window,
                               int key,
                               int scancode,
                               int action,
                               int mods)
{
    GLFWApp::_keyCallback(window, key, scancode, action, mods);
    if (_scene)
    {
        if (action == GLFW_PRESS)
        {
            switch (key)
            {
            case GLFW_KEY_SPACE:
                _anim = !_anim;
                break;
            }
        }
    }
}

}  // namespace examples
