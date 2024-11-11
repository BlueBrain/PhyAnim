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

#include "MoveMeshApp.h"

#include <iostream>

using namespace phyanim;

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
    _bbFactor = 1;

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

    // for (auto aabb : _aabbs)
    //     std::cout << "radius: " << aabb->radius() << std::endl;

    anim::CollisionDetection::computeCollisions(_meshes, 0.0f);
    _colorMeshes();
    if (_aabbs.size() > 0)
    {
        std::cout << "Collision id: " << _collisionId << std::endl;
        //   << " radius: " << _aabbs[0]->radius() << std::endl;
        auto limit = *_aabbs[0];
        limit.resize(2.0f);
        _setCameraPos(limit);
    }
}

void MoveMeshApp::_colorMeshes()
{
    geometry::Vec3 baseColor(0, 0.8, 0.8);
    geometry::Vec3 baseSelectedColor(0.0, 0.2, 0.8);

    geometry::Vec3 collisionColor(0.8, 0.2, 0.0);
    geometry::Vec3 collisionSelectedColor(0.8, 0.2, 0.0);

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < _meshes.size(); ++i)
    {
        geometry::Vec3 color = baseColor;
        geometry::Vec3 collColor = collisionColor;
        geometry::MeshPtr mesh = _meshes[i];
        if (mesh == _animMesh)
        {
            color = baseSelectedColor;
            collColor = collisionSelectedColor;
        }

        uint64_t nodeSize = mesh->nodes.size();
        std::vector<float> colors(nodeSize * 3);

        for (uint64_t j = 0; j < nodeSize; ++j)
        {
            geometry::Vec3 nodeColor = color;
            if (mesh->nodes[j]->collide) nodeColor = collColor;
            colors[j * 3] = nodeColor.x;
            colors[j * 3 + 1] = nodeColor.y;
            colors[j * 3 + 2] = nodeColor.z;
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
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        _mouseX = mouseX;
        _mouseY - mouseY;
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
                    for (auto mesh : _meshes)
                        geometry::clearCollision(mesh->nodes);
                    _aabbs = anim::CollisionDetection::collisionBoundingBoxes(
                        _meshes, _bbFactor);
                    _sortAABBs(_aabbs);
                    std::cout << "Number of collisions: " << _aabbs.size()
                              << std::endl;
                    anim::CollisionDetection::computeCollisions(_meshes, 0.0);
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
}

}  // namespace examples
