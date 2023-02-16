#include "MoveMeshApp.h"

#include <iomanip>
#include <iostream>

int main(int argc, char* argv[])
{
    examples::MoveMeshApp app(argc, argv);
    app.run();

    return 0;
}

namespace examples
{
MoveMeshApp::MoveMeshApp(int argc, char** argv)
    : GLFWApp(argc, argv)
    , _animMesh(nullptr)
    , _renderMesh(nullptr)
{
}

void MoveMeshApp::_actionLoop()
{
    _bbFactor = 15;

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
        else
            std::cerr << "Unknown file format: " << _args[i] << std::endl;
    }

    _loadMeshes(_fileNames);

    for (auto aabb : _aabbs)
        std::cout << "radius: " << aabb->radius() << std::endl;

    phyanim::CollisionDetection::computeCollisions(_meshes, 0.0);
    _colorMeshes();
    if (_aabbs.size() > 0)
    {
        std::cout << "Collision id: " << _collisionId
                  << " radius: " << _aabbs[0]->radius() << std::endl;
        _setCameraPos(*_aabbs[0]);
    }
}

void MoveMeshApp::_colorMeshes()
{
    phyanim::Vec3 baseColor(0, 0.8, 0.8);
    phyanim::Vec3 baseSelectedColor(0.0, 0.2, 0.8);

    phyanim::Vec3 collisionColor(0.8, 0.2, 0.0);
    phyanim::Vec3 collisionSelectedColor(0.8, 0.2, 0.0);

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < _meshes.size(); ++i)
    {
        phyanim::Vec3 color = baseColor;
        phyanim::Vec3 collColor = collisionColor;
        phyanim::MeshPtr mesh = _meshes[i];
        if (mesh == _animMesh)
        {
            color = baseSelectedColor;
            collColor = collisionSelectedColor;
        }

        uint64_t nodeSize = mesh->nodes.size();
        std::vector<double> colors(nodeSize * 3);

        for (uint64_t j = 0; j < nodeSize; ++j)
        {
            phyanim::Vec3 nodeColor = color;
            if (mesh->nodes[j]->collide) nodeColor = collColor;
            colors[j * 3] = nodeColor.x();
            colors[j * 3 + 1] = nodeColor.y();
            colors[j * 3 + 2] = nodeColor.z();
        }
        _scene->meshes[i]->uploadColors(colors);
    }
}

void MoveMeshApp::_keyCallback(GLFWwindow* window,
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

void MoveMeshApp::_mouseButtonCallback(GLFWwindow* window,
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
                uint32_t pickedId = _scene->picking(_mouseX, _mouseY);
                if (pickedId != 0)
                {
                    _animMesh = _meshes[pickedId - 1];
                    _renderMesh = _scene->meshes[pickedId - 1];
                    _colorMeshes();
                }
            }
            else if (action == GLFW_RELEASE)
            {
                _leftButtonPressed = false;
                if (_animMesh)
                {
                    _animMesh->boundingBox->update();
                    _aabbs =
                        phyanim::CollisionDetection::collisionBoundingBoxes(
                            _meshes, _bbFactor);
                    _sortAABBs(_aabbs);
                    std::cout << "Number of collisions: " << _aabbs.size()
                              << std::endl;
                    phyanim::CollisionDetection::computeCollisions(_meshes,
                                                                   0.0);
                    _animMesh = nullptr;
                    _renderMesh = nullptr;
                    _colorMeshes();
                }
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

void MoveMeshApp::_mousePositionCallback(GLFWwindow* window,
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
            phyanim::Vec3(-diffX * _scene->cameraDistance() * 0.01f,
                          diffY * _scene->cameraDistance() * 0.01f, 0.0);
        if (_leftButtonPressed)
        {
            if (_animMesh)
            {
                dxyz = _scene->cameraRotation() * dxyz;
                for (auto node : _animMesh->nodes) node->position -= dxyz;
                setGeometry(_renderMesh, _animMesh->nodes);
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
