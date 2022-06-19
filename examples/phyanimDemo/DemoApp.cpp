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
        else if (_args[i].find(".off") != std::string::npos)
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

    for (auto aabb : _aabbs)
        std::cout << "rdaius: " << aabb->radius() << std::endl;

    std::cout << "Number of collisions: " << _aabbs.size() << std::endl;
    phyanim::CollisionDetection::computeCollisions(_meshes, 0.0, true);
    _coloredMeshes();
    _collisionId = 0;
    if (_aabbs.size() > 0)
    {
        std::cout << "Collision id: " << _collisionId
                  << " radius: " << _aabbs[0]->radius() << std::endl;
        // _setCameraPos(*_aabbs[0], false);
    }
}

void DemoApp::_coloredMeshes()
{
    phyanim::Vec3 baseColor(0.4, 0.4, 0.8);
    phyanim::Vec3 baseSelectedColor(0.0, 0.0, 1.0);

    phyanim::Vec3 collisionColor(1.0, 0.0, 0.0);
    phyanim::Vec3 collisionSelectedColor(1.0, 0.0, 0.0);

    for (auto mesh : _meshes)
    {
        phyanim::Vec3 color = baseColor;
        phyanim::Vec3 collColor = collisionColor;

        if (mesh == _mesh)
        {
            color = baseSelectedColor;
            collColor = collisionSelectedColor;
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
                    phyanim::CollisionDetection::computeCollisions(_meshes, 0.0,
                                                                   true);
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
            phyanim::Vec3(-diffX * _cameraPosInc, diffY * _cameraPosInc, 0.0);
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
