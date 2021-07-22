#include "MoveMeshApp.h"

#include <iostream>

namespace examples
{

MoveMeshApp::MoveMeshApp()
    : GLFWApp()
    , _editing(false)
    , _meshId(0)
    , _finished(false)
{
    
}

void MoveMeshApp::init(int argc, char** argv)
{
    _scene = new Scene();


    phyanim::AABB limits;
    for(uint32_t i = 1; i < argc; ++i)
    {
        phyanim::DrawableMesh* mesh;
        std::string file(argv[i]);
    
        if (file.find(".node") != std::string::npos)
        {
            std::string file1(argv[i+1]);
            mesh = new phyanim::DrawableMesh();
            mesh->load(file, file1);
            ++i;
        }
        else if (file.find(".tet") != std::string::npos)
        {
            mesh = new phyanim::DrawableMesh();
            mesh->load(file);
        }
        else
        {
            std::cerr << "Unknown file format: " << file << std::endl;
            exit(-1);
        }

        if (mesh)
        {
            mesh->upload();
            limits.update(mesh->aabb->root->aabb);
            _meshes.push_back(mesh);
            _scene->addMesh(mesh);
        }
    }
    _setCameraPos(limits);
}

void MoveMeshApp::loop()
{
    while(!glfwWindowShouldClose(_window))
    {

        if(!_editing && !_finished)
        {
            if (_meshId < _meshes.size())
            {
                _mesh = _meshes[_meshId];
                ++_meshId;
                _editing = true;
            }
            else
            {
                for (uint32_t i = 0; i < _meshes.size(); ++i)
                {
                    std::string file("result_" + std::to_string(i+1) + ".tet");
                    _meshes[i]->write(file);
                    std::cout << "File " << file << " saved." << std::endl;
                }
                _finished = true;
            }
        }
        _scene->render();
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    glfwTerminate();
}

void MoveMeshApp::_keyCallback(GLFWwindow* window, int key, int scancode,
                                int action, int mods)
{
    GLFWApp::_keyCallback(window, key, scancode, action, mods);

    phyanim::Vec3 dxyz(0.0, 0.0, 0.0);
    double dx = 0.1;
    bool meshDisplaced = false;
    switch(key){
    case 'I':
        dxyz += phyanim::Vec3(0.0, 0.0, -dx);
        meshDisplaced = true;
        break;
    case 'K':
        dxyz += phyanim::Vec3(0.0, 0.0, dx);
        meshDisplaced = true;
        break;
    case 'J':
        dxyz += phyanim::Vec3(-dx, 0.0, 0.0);
        meshDisplaced = true;
        break;
    case 'L':
        dxyz += phyanim::Vec3(dx, 0.0, 0.0);
        meshDisplaced = true;
        break;
    case 'O':
        dxyz += phyanim::Vec3(0, dx, 0.0);
        meshDisplaced = true;
        break;
    case 'P':
        dxyz += phyanim::Vec3(0, -dx, 0.0);
        meshDisplaced = true;
        break;
    }
    if (meshDisplaced) {
        for (auto node: _mesh->nodes)
        {
            node->position += dxyz;
        }
        auto mesh = dynamic_cast<phyanim::DrawableMesh*>(_mesh);
        mesh->uploadNodes();
    }
    if (action == GLFW_PRESS)
    {
        switch(key){
        case GLFW_KEY_SPACE:
            _editing = false;
            break;
        }
    }
}


}
