#include "AnimMeshApp.h"

#include <iostream>

namespace examples
{

AnimMeshApp::AnimMeshApp()
    : GLFWApp()
    , _anim(false)
{
    
}

void AnimMeshApp::init(int argc, char** argv)
{
    _scene = new Scene();
    _animSys = new phyanim::ImplicitFEMSystem(0.01);
    _collisionSys = new phyanim::CollisionDetection();

    std::string usage = std::string("Usage error: Use ") + std::string(argv[0]) +
            std::string(" mesh_file[.tet|.node .ele]");
    if (argc < 2)
    {
        std::cerr << usage << std::endl;
        exit(-1);
    }

    _mesh = new phyanim::DrawableMesh();
    std::string file(argv[1]);
    
    if (file.find(".node") != std::string::npos)
    {
        std::string file1(argv[2]);
        _mesh->load(file, file1);
    }
    else if (file.find(".tet") != std::string::npos)
    {
        _mesh->load(file);
    }
    else
    {
        std::cerr << usage << std::endl;
        exit(-1);
    }
    _mesh->upload();
    _scene->addMesh(_mesh);
    _animSys->addMesh(_mesh);
    phyanim::Meshes collMeshes(1);
    collMeshes[0] = _mesh;
    _collisionSys->dynamicMeshes(collMeshes);
}

void AnimMeshApp::loop()
{
    while(!glfwWindowShouldClose(_window))
    {
        if (_anim)
        {
            _mesh->nodesForceZero();
            _collisionSys->update();
            _animSys->step();
            _collisionSys->checkLimitsCollision();
            _mesh->uploadNodes();
        }
        _scene->render();
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    glfwTerminate();
}

void AnimMeshApp::_keyCallback(GLFWwindow* window, int key, int scancode,
                               int action, int mods)
{
    GLFWApp::_keyCallback(window, key, scancode, action, mods);
    if (_scene)
    {
        if (action == GLFW_PRESS)
        {
            switch(key)
            {
            case GLFW_KEY_SPACE:
                _anim = !_anim;
                break;
            }
        }
    }
}

}
