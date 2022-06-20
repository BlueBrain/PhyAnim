#include "DemoApp.h"

#include <iomanip>
#include <iostream>

int main(int argc, char* argv[])
{
    examples::DemoApp app(argc, argv);
    app.run();

    return 0;
}

namespace examples
{
DemoApp::DemoApp(int argc, char** argv) : GLFWApp(argc, argv), _mesh(nullptr) {}

void DemoApp::_actionLoop()
{
    _bbFactor = 2;

    for (uint32_t i = 0; i < _args.size(); ++i)
    {
        size_t extensionPos;
        if (_args[i].compare("-bb") == 0)
        {
            ++i;
            _bbFactor = std::stoi(_args[i]);
        }
        else if (_args[i].find(".tet") != std::string::npos)
        {
            _fileNames.push_back(_args[i]);
        }
        else if (_args[i].find(".off") != std::string::npos)
        {
            _fileNames.push_back(_args[i]);
        } 
        else if (_args[i].find(".obj") != std::string::npos)
        {
            _fileNames.push_back(_args[i]);
        }
        else
            std::cerr << "Unknown file format: " << _args[i] << std::endl;
    }

    _loadMeshes(_fileNames);
    for (auto mesh : _meshes)
        _scene->addMesh(dynamic_cast<phyanim::DrawableMesh*>(mesh));

    _aabbs =
        phyanim::CollisionDetection::collisionBoundingBoxes(_meshes, _bbFactor);
    _sortAABBs(_aabbs);

    std::cout << "Number of collisions: " << _aabbs.size() << std::endl;
    _checkCollisions();
    _coloredMeshes();
    _collisionId = 0;
}

void DemoApp::_checkCollisions()
{
    for (auto mesh: _meshes)
        for (auto node: mesh->nodes)
            node->collide = false;

    for (auto aabb: _aabbs)
        for (auto mesh : _meshes)
            for (auto node: mesh->nodes)
                if (aabb->isInside(node->position))
                    node->collide = true;
}


void DemoApp::_coloredMeshes()
{

    for (uint32_t meshId = 0; meshId < _meshes.size(); ++meshId)
    {
        auto mesh = _meshes[meshId];
        phyanim::Vec3 color = _palette.color(meshId);
        phyanim::Vec3 collColor = _palette.collisionColor();
        if (mesh == _mesh)
        {
            color *= 1.5f;    
            collColor *= 2.0f;
        }

        uint64_t nodeSize = mesh->nodes.size();
        for (uint64_t i = 0; i < nodeSize; ++i)
        {
            auto node = mesh->nodes[i];
            phyanim::Vec3 nodeColor = color;
            if (node->collide) nodeColor = collColor;
            node->color = nodeColor;
        }
        dynamic_cast<phyanim::DrawableMesh*>(mesh)->updatedColors = true;
    }
}

void DemoApp::_keyCallback(GLFWwindow* window,
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
            case GLFW_KEY_ENTER:
                _writeMeshes(_meshes, _fileNames);
                break;
            }
        }
    }
}

void DemoApp::_mouseButtonCallback(GLFWwindow* window,
                                   int button,
                                   int action,
                                   int mods)
{
    if (_scene)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
            {
                _leftButtonPressed = true;
                glfwGetCursorPos(window, &_mouseX, &_mouseY);
                uint32_t pickedId = _scene->picking(_mouseX, _mouseY);
                if (pickedId != 0) _mesh = _meshes[pickedId - 1];
                _coloredMeshes();
            }
            else if (action == GLFW_RELEASE)
            {
                _leftButtonPressed = false;
                if (_mesh)
                {
                    _mesh->boundingBox->update();
                    _aabbs =
                        phyanim::CollisionDetection::collisionBoundingBoxes(
                            _meshes, _bbFactor);
                    _sortAABBs(_aabbs);
                    std::cout << "Number of collisions: " << _aabbs.size()
                              << std::endl;
                    _checkCollisions();
                    _mesh = nullptr;
                    _coloredMeshes();
                }
            }
        }
        if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            if (action == GLFW_PRESS)
            {
                _middleButtonPressed = true;

                glfwGetCursorPos(window, &_mouseX, &_mouseY);
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

                glfwGetCursorPos(window, &_mouseX, &_mouseY);
            }
            else if (action == GLFW_RELEASE)
            {
                _rightButtonPressed = false;
            }
        }
    }
}

void DemoApp::_mousePositionCallback(GLFWwindow* window,
                                     double xpos,
                                     double ypos)
{
    if (_scene)
    {
        double diffX = xpos - _mouseX;
        double diffY = ypos - _mouseY;
        _mouseX = xpos;
        _mouseY = ypos;
        phyanim::Vec3 dxyz =
            phyanim::Vec3(-diffX * _scene->cameraDistance() * 0.001f, diffY * _scene->cameraDistance() * 0.001f, 0.0);
        if (_leftButtonPressed)
        {
            if (_mesh)
            {
                dxyz = _scene->cameraRotation() * dxyz;
                for (auto node : _mesh->nodes) node->position -= dxyz;
                dynamic_cast<phyanim::DrawableMesh*>(_mesh)->updatedPositions =
                    true;
            }
        }
        if (_middleButtonPressed)
        {
            _scene->displaceCamera(dxyz);
        }
        if (_rightButtonPressed)
        {
            _scene->rotateCamera(-diffY * 0.005, -diffX * 0.005);
        }
    }
}

}  // namespace examples
