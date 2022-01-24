#include "SceneGeneratorApp.h"

#include <SceneGenerator.h>

#include <iomanip>
#include <iostream>

int main(int argc, char* argv[])
{
    examples::SceneGeneratorApp app(argc, argv);
    app.run();

    return 0;
}

namespace examples
{
SceneGeneratorApp::SceneGeneratorApp(int argc, char** argv)
    : GLFWApp(argc, argv)
    , _mesh(nullptr)
{
}

void SceneGeneratorApp::_actionLoop()
{
    uint32_t numOutMeshes = 10;
    _bbFactor = 15.0;
    double limitFactor = 1.0;
    uint32_t maxCollisions = 5;
    double maxCollisionRadius = 4.0;

    std::vector<std::string> files;

    for (uint32_t i = 0; i < _args.size(); ++i)
    {
        if (_args[i].compare("-n") == 0)
        {
            ++i;
            numOutMeshes = std::stoi(_args[i]);
        }
        else if (_args[i].compare("-c") == 0)
        {
            ++i;
            maxCollisions = std::stoi(_args[i]);
        }
        else if (_args[i].compare("-cr") == 0)
        {
            ++i;
            maxCollisionRadius = std::stoi(_args[i]);
        }
        else if (_args[i].compare("-bb") == 0)
        {
            ++i;
            _bbFactor = std::stod(_args[i]);
        }
        else if (_args[i].compare("-l") == 0)
        {
            ++i;
            limitFactor = std::stod(_args[i]);
        }
        else if (_args[i].find(".tet") != std::string::npos)
            files.push_back(_args[i]);
        else
            std::cerr << "Unknown file format: " << _args[i] << std::endl;
    }

    phyanim::Meshes meshes;
    phyanim::AxisAlignedBoundingBox limits;

    std::cout << std::fixed << std::setprecision(2);
    double progress = 0.0;
    std::cout << "\rLoading files " << progress << "%" << std::flush;

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < files.size(); ++i)
    {
        phyanim::DrawableMesh* mesh = new phyanim::DrawableMesh();
        mesh->load(files[i]);
#pragma omp critical
        {
            if (mesh)
            {
                meshes.push_back(mesh);
                limits.unite(
                    phyanim::AxisAlignedBoundingBox(mesh->surfaceTriangles));
            }

            progress += 100.0f / files.size();
            std::cout << "\rLoading files " << progress << "%" << std::flush;
        }
    }
    std::cout << std::endl;

    _setCameraPos(limits);
    phyanim::Vec3 center = limits.center();
    phyanim::Vec3 axis = (limits.upperLimit() - center) * limitFactor;
    limits.lowerLimit(center - axis);
    limits.upperLimit(center + axis);

    auto sceneMeshes = examples::SceneGenerator::generate(
        meshes, numOutMeshes, limits, _bbFactor, maxCollisions);

    for (auto mesh : sceneMeshes)
    {
        mesh->boundingBox =
            new phyanim::HierarchicalAABB(mesh->surfaceTriangles);
        _meshes.push_back(mesh);
        _scene->addMesh(dynamic_cast<phyanim::DrawableMesh*>(mesh));
        limits.unite(*mesh->boundingBox);
    }

    for (auto mesh : meshes)
    {
        mesh->clearData();
        delete mesh;
    }
    meshes.clear();

    phyanim::CollisionDetection::computeCollisions(_meshes, 0.0, true);
    _coloredMeshes();

    _aabbs =
        phyanim::CollisionDetection::collisionBoundingBoxes(_meshes, _bbFactor);
    _sortAABBs(_aabbs);
    std::cout << "Number of collisions: " << _aabbs.size() << std::endl;
    _collisionId = 0;
    if (_aabbs.size() > 0)
    {
        std::cout << "Collision id: " << _collisionId << std::endl;
        _setCameraPos(*_aabbs[0], false);
    }
}

void SceneGeneratorApp::_coloredMeshes()
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

void SceneGeneratorApp::_keyCallback(GLFWwindow* window,
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
                double progress = 0.0;
                std::cout << "\rSaving files " << progress << "%" << std::flush;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
                for (uint32_t i = 0; i < _meshes.size(); ++i)
                {
                    std::string fileName(std::to_string(i) + "_gen.tet");
                    _meshes[i]->write(fileName);
#pragma omp critical
                    {
                        progress += 100.0f / _meshes.size();
                        std::cout << "\rSaving files " << progress << "%"
                                  << std::flush;
                    }
                }
                std::cout << std::endl;
                break;
            }
        }
    }
}

void SceneGeneratorApp::_mouseButtonCallback(GLFWwindow* window,
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
                    phyanim::CollisionDetection::computeCollisions(_meshes, 0.0,
                                                                   true);
                    _sortAABBs(_aabbs);
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

void SceneGeneratorApp::_mousePositionCallback(GLFWwindow* window,
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
}  // namespace examples

}  // namespace examples
