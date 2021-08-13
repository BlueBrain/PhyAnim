#include "AnimMeshApp.h"

#include <ExplicitFEMSystem.h>
#include <ExplicitMassSpringSystem.h>
#include <ImplicitFEMSystem.h>
#include <ImplicitMassSpringSystem.h>

#include <iostream>

namespace examples
{
AnimMeshApp::AnimMeshApp()
    : _anim(false)
    , _pauseColor(0.4, 0.4, 1.0)
    , _animColor(1, 0.6, 0.6)
{
}

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
    _collisionStiffness = 100.0;
    double dt = 0.01;
    double damping = 1.0;
    double density = 1.0;
    SimSystem simSystem = imfem;

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
            _collisionStiffness = std::atof(argv[i]);
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

    for (uint64_t i = 0; i < files.size(); ++i)
    {
        auto mesh =
            new phyanim::DrawableMesh(stiffness, density, damping, poisson);
        std::string file = files[i];

        if (file.find(".node") != std::string::npos)
        {
            ++i;
            std::string file1 = files[i];
            mesh->load(file, file1);
            mesh->compute(createEdges);
        }
        else if (file.find(".tet") != std::string::npos)
        {
            mesh->load(file);
            mesh->compute(createEdges);
        }
        else
        {
            std::cerr << "Error: Unknown format file." << std::endl;
            exit(-1);
        }

        _meshes.push_back(mesh);
        mesh->boundingBox =
            new phyanim::HierarchicalAABB(mesh->surfaceTriangles);
        mesh->upload(_pauseColor);

        _limits.unite(*mesh->boundingBox);
        _scene->addMesh(mesh);
    }

    _animSys->preprocessMesh(_meshes);

    phyanim::Vec3 halfSides = _limits.upperLimit() - _limits.center();
    _limits.lowerLimit(_limits.lowerLimit() - halfSides);
    _limits.upperLimit(_limits.upperLimit() + halfSides);
    _setCameraPos(_limits);
}

void AnimMeshApp::loop()
{
    while (!glfwWindowShouldClose(_window))
    {
        if (_anim)
        {
            for (uint32_t i = 0; i < _meshes.size(); ++i)
            {
                auto mesh = dynamic_cast<phyanim::DrawableMesh*>(_meshes[i]);
                mesh->nodesForceZero();
            }
            phyanim::CollisionDetection::computeCollisions(
                _meshes, _collisionStiffness, true);
            _animSys->step(_meshes);
            for (auto mesh : _meshes)
            {
                mesh->boundingBox->update();
            }
            phyanim::CollisionDetection::computeCollisions(_meshes, _limits);
            for (auto mesh : _meshes)
            {
                auto drawMesh = dynamic_cast<phyanim::DrawableMesh*>(mesh);
                drawMesh->uploadPositions();
            }
            _scene->updateColors();
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
                // phyanim::Vec3 color;
                // if (_anim)
                // {
                //     color = _animColor;
                // }
                // else
                // {
                //     color = _pauseColor;
                // }
                // for (auto mesh : _meshes)
                // {
                //     dynamic_cast<phyanim::DrawableMesh*>(mesh)->uploadColors(
                //         color);
                // }
                break;
            }
        }
    }
}

}  // namespace examples
