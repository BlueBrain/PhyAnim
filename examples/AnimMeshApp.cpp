#include "AnimMeshApp.h"

#include <iostream>

namespace examples
{
AnimMeshApp::AnimMeshApp() : GLFWApp(), _anim(false) {}

void AnimMeshApp::init(int argc, char** argv)
{
    _scene = new Scene();
    _animSys = new phyanim::ImplicitFEMSystem(0.01);
    _collisionSys = new phyanim::CollisionDetection(100);

    std::string usage = std::string("Usage error: Use ") +
                        std::string(argv[0]) +
                        std::string(" mesh_file[.tet|.node .ele]");
    if (argc < 2)
    {
        std::cerr << usage << std::endl;
        exit(-1);
    }

    phyanim::AABB limits;
    for (uint32_t i = 1; i < argc; ++i)
    {
        auto mesh = new phyanim::DrawableMesh();
        std::string file(argv[i]);

        if (file.find(".node") != std::string::npos)
        {
            std::string file1(argv[i + 1]);
            mesh->load(file, file1);
            ++i;
        }
        else if (file.find(".tet") != std::string::npos)
        {
            mesh->load(file);
        }
        else
        {
            std::cerr << usage << std::endl;
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
