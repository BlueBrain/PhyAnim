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
    , _mesh(nullptr)
{
}

void MoveMeshApp::_actionLoop()
{
    _bbFactor = 1.0001;
    std::vector<std::string> files;

    for (uint32_t i = 0; i < _args.size(); ++i)
    {
        size_t extensionPos;
        if (_args[i].compare("-bb") == 0)
        {
            ++i;
            _bbFactor = std::stoi(_args[i]);
        }
        else if ((extensionPos = _args[i].find(".tet")) != std::string::npos)
        {
            files.push_back(_args[i]);
            auto fileName = _args[i].substr(0, extensionPos);
            _fileNames.push_back(fileName);
        }
        else
            std::cerr << "Unknown file format: " << _args[i] << std::endl;
    }

    std::cout << std::fixed << std::setprecision(2);
    double progress = 0.0;
    std::cout << "\rLoading files " << progress << "%" << std::flush;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < files.size(); ++i)
    {
        phyanim::DrawableMesh* mesh = new phyanim::DrawableMesh();
        mesh->load(files[i]);
        if (mesh)
        {
            mesh->boundingBox =
                new phyanim::HierarchicalAABB(mesh->surfaceTriangles);

            _limits.unite(*mesh->boundingBox);
            _meshes.push_back(mesh);
            _scene->addMesh(mesh);
            _setCameraPos(_limits);
        }
#pragma omp critical
        {
            progress += 100.0f / files.size();
            std::cout << "\rLoading files " << progress << "%" << std::flush;
        }
    }
    std::cout << std::endl;

    _aabbs =
        phyanim::CollisionDetection::collisionBoundingBoxes(_meshes, _bbFactor);
    std::cout << "Number of collisions: " << _aabbs.size() << std::endl;
    phyanim::CollisionDetection::computeCollisions(_meshes, 0.0, true);
    _coloredMeshes();
    _collisionId = 0;
    if (_aabbs.size() > 0)
    {
        std::cout << "Collision id: " << _collisionId << std::endl;
        _setCameraPos(*_aabbs[0], false);
    }
}

void MoveMeshApp::_coloredMeshes()
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

void MoveMeshApp::_keyCallback(GLFWwindow* window,
                               int key,
                               int scancode,
                               int action,
                               int mods)
{
    if (_scene)
    {
        double dx = _cameraPosInc * 2.0;
        phyanim::Vec3 dxyz(0.0, 0.0, 0.0);
        bool cameraDisplaced = false;

        if (action == GLFW_PRESS)
        {
            switch (key)
            {
            case 'M':
                _scene->changeRenderMode();
                break;
            case 'F':
                _scene->showFPS = !_scene->showFPS;
                break;
            case GLFW_KEY_ENTER:
                std::cout << "Saving files:" << std::endl;

                for (uint32_t i = 0; i < _meshes.size(); ++i)
                {
                    std::string fileName(_fileNames[i] + ".tet");
                    std::cout << "\t Saved " << fileName << std::endl;
                    _meshes[i]->write(fileName);
                }
                break;
            case GLFW_KEY_SPACE:
                uint32_t aabbsSize = _aabbs.size();
                if (aabbsSize > 0)
                {
                    _collisionId = (_collisionId + 1) % aabbsSize;
                    std::cout << "Collision id: " << _collisionId << std::endl;
                    _setCameraPos(*_aabbs[_collisionId], false);
                }
                break;
            }
        }

        switch (key)
        {
        case 'W':
            dxyz += phyanim::Vec3(0.0, 0.0, -dx);
            cameraDisplaced = true;
            break;
        case 'S':
            dxyz += phyanim::Vec3(0.0, 0.0, dx);
            cameraDisplaced = true;
            break;
        case 'A':
            dxyz += phyanim::Vec3(-dx, 0.0, 0.0);
            cameraDisplaced = true;
            break;
        case 'D':
            dxyz += phyanim::Vec3(dx, 0.0, 0.0);
            cameraDisplaced = true;
            break;
        }
        if (cameraDisplaced)
        {
            if (_mesh)
            {
                dxyz = _scene->cameraRotation() * dxyz;
                for (auto node : _mesh->nodes) node->position += dxyz;
                dynamic_cast<phyanim::DrawableMesh*>(_mesh)->updatedPositions =
                    true;
                _mesh->boundingBox->update();
                phyanim::CollisionDetection::computeCollisions(_meshes, 0.0,
                                                               true);
                _coloredMeshes();
            }
            else
                _scene->displaceCamera(dxyz);
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
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
            {
                _leftButtonPressed = true;

                glfwGetCursorPos(window, &_mouseX, &_mouseY);

                uint32_t pickedId = _scene->picking(_mouseX, _mouseY);

                if (pickedId != 0)
                {
                    _mesh = _meshes[pickedId - 1];
                    _pickingTime = std::chrono::steady_clock::now();
                };

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
            phyanim::Vec3(-diffX * _cameraPosInc, diffY * _cameraPosInc, 0.0);
        if (_leftButtonPressed)
        {
            if (_mesh)
            {
                dxyz = _scene->cameraRotation() * dxyz;
                for (auto node : _mesh->nodes) node->position -= dxyz;
                dynamic_cast<phyanim::DrawableMesh*>(_mesh)->updatedPositions =
                    true;

                // auto currentTime = std::chrono::steady_clock::now();
                // auto elapsedTime =
                //     (std::chrono::duration<double>(currentTime -
                //     _pickingTime))
                //         .count();
                // if (elapsedTime > 0.1)
                // {
                //     _mesh->boundingBox->update();
                //     phyanim::CollisionDetection::computeCollisions(_meshes,
                //     0.0,
                //                                                    true);
                //     _coloredMeshes();
                //     _pickingTime = currentTime;
                // }
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
