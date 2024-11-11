/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/PhyAnim>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "SceneGeneratorApp.h"

#include <iomanip>
#include <iostream>

#include "../common/SceneGenerator.h"

using namespace phyanim;

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
    , _animMesh(nullptr)
    , _renderMesh(nullptr)
{
}

void SceneGeneratorApp::_actionLoop()
{
    uint32_t numOutMeshes = 10;
    _bbFactor = 15.0;
    float limitFactor = 1.0;
    uint32_t maxCollisions = 5;
    float maxCollisionRadius = 4.0;

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

    geometry::Meshes meshes;
    geometry::AxisAlignedBoundingBox limits;

    std::cout << std::fixed << std::setprecision(2);
    float progress = 0.0;
    std::cout << "\rLoading files " << progress << "%" << std::flush;

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint64_t i = 0; i < files.size(); ++i)
    {
        geometry::Mesh* mesh = new geometry::Mesh();
        mesh->load(files[i]);
#pragma omp critical
        {
            if (mesh)
            {
                meshes.push_back(mesh);
                limits.unite(
                    geometry::AxisAlignedBoundingBox(mesh->surfaceTriangles));
            }

            progress += 100.0f / files.size();
            std::cout << "\rLoading files " << progress << "%" << std::flush;
        }
    }
    std::cout << std::endl;

    _setCameraPos(limits);
    geometry::Vec3 center = limits.center();
    geometry::Vec3 axis = (limits.upperLimit() - center) * limitFactor;
    limits.lowerLimit(center - axis);
    limits.upperLimit(center + axis);

    _meshes = examples::SceneGenerator::generate(meshes, numOutMeshes, limits,
                                                 _bbFactor, maxCollisions);

    for (auto mesh : _meshes)
    {
        mesh->boundingBox =
            new geometry::HierarchicalAABB(mesh->surfaceTriangles);

        _scene->meshes.push_back(
            graphics::generateMesh(mesh->nodes, mesh->surfaceTriangles));
        limits.unite(*mesh->boundingBox);
    }

    for (auto mesh : meshes)
    {
        mesh->clearData();
        delete mesh;
    }
    meshes.clear();

    anim::CollisionDetection::computeCollisions(_meshes, 0.0);
    _coloredMeshes();

    _aabbs =
        anim::CollisionDetection::collisionBoundingBoxes(_meshes, _bbFactor);
    _sortAABBs(_aabbs);
    std::cout << "Number of collisions: " << _aabbs.size() << std::endl;
    _collisionId = 0;
    if (_aabbs.size() > 0)
    {
        std::cout << "Collision id: " << _collisionId << std::endl;
        _setCameraPos(*_aabbs[0]);
    }
}

void SceneGeneratorApp::_coloredMeshes()
{
    geometry::Vec3 baseColor(0.4, 0.4, 0.8);
    geometry::Vec3 baseSelectedColor(0.0, 0.0, 1.0);

    geometry::Vec3 collisionColor(1.0, 0.0, 0.0);
    geometry::Vec3 collisionSelectedColor(1.0, 0.0, 0.0);

    for (uint32_t i = 0; i < _meshes.size(); ++i)
    {
        auto animMesh = _meshes[i];
        auto renderMesh = _scene->meshes[i];

        geometry::Vec3 color = baseColor;
        geometry::Vec3 collColor = collisionColor;
        if (animMesh == _animMesh)
        {
            color = baseSelectedColor;
            collColor = collisionSelectedColor;
        }
        setColorByCollision(renderMesh, animMesh->nodes, color, collColor);
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
                float progress = 0.0;
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
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        _mouseX = mouseX;
        _mouseY = mouseY;

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
                    _pickingTime = std::chrono::steady_clock::now();
                };

                _coloredMeshes();
            }
            else if (action == GLFW_RELEASE)
            {
                _leftButtonPressed = false;
                if (_animMesh)
                {
                    _animMesh->boundingBox->update();
                    anim::CollisionDetection::computeCollisions(_meshes, 0.0);
                    _sortAABBs(_aabbs);
                    _animMesh = nullptr;
                    _renderMesh = nullptr;
                    _coloredMeshes();
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

void SceneGeneratorApp::_mousePositionCallback(GLFWwindow* window,
                                               float xpos,
                                               float ypos)
{
    if (_scene)
    {
        float diffX = xpos - _mouseX;
        float diffY = ypos - _mouseY;
        _mouseX = xpos;
        _mouseY = ypos;
        geometry::Vec3 dxyz =
            geometry::Vec3(-diffX * _scene->cameraDistance() * 0.001f,
                           diffY * _scene->cameraDistance() * 0.001f, 0.0);
        if (_leftButtonPressed)
        {
            if (_animMesh)
            {
                dxyz = dxyz * _scene->cameraRotation();
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
}  // namespace examples

}  // namespace examples
