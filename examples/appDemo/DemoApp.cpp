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

#include "DemoApp.h"

#include <iomanip>
#include <iostream>

using namespace phyanim;

int main(int argc, char* argv[])
{
    examples::DemoApp app(argc, argv);
    app.run();

    return 0;
}

namespace examples
{
DemoApp::DemoApp(int argc, char** argv)
    : GLFWApp(argc, argv)
    , _animMesh(nullptr)
    , _renderMesh(nullptr)
{
}

void DemoApp::_actionLoop()
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
        if (_args[i].compare("-fov") == 0)
        {
            ++i;
            _scene->cameraFov(std::stof(_args[i]));
        }
        if (_args[i].compare("-fps") == 0)
        {
            _scene->showFPS = true;
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

    std::string texPath(DATAPATH);
    texPath.append("/img/skyCells/");
    _scene->setSky(texPath);

    _loadMeshes(_fileNames);
    _checkCollisions();
    _coloredMeshes();
}

void DemoApp::_checkCollisions()
{
    _aabbs =
        anim::CollisionDetection::collisionBoundingBoxes(_meshes, _bbFactor);
    _sortAABBs(_aabbs);

    for (auto mesh : _meshes)
        for (auto node : mesh->nodes) node->collide = false;

    for (auto aabb : _aabbs)
        for (auto mesh : _meshes)
            for (auto node : mesh->nodes)
                if (aabb->isInside(node->position)) node->collide = true;
}

void DemoApp::_coloredMeshes()
{
    for (uint32_t meshId = 0; meshId < _meshes.size(); ++meshId)
    {
        auto mesh = _meshes[meshId];
        geometry::Vec3 color = _palette.color(meshId);
        geometry::Vec3 collColor = _palette.collisionColor();
        if (mesh == _animMesh)
        {
            color *= 1.5f;
            collColor *= 2.0f;
        }

        uint64_t nodeSize = mesh->nodes.size();
        std::vector<float> colors(nodeSize * 3);
        for (uint64_t i = 0; i < nodeSize; ++i)
        {
            auto node = mesh->nodes[i];
            geometry::Vec3 nodeColor = color;
            if (node->collide) nodeColor = collColor;
            colors[i * 3] = nodeColor.x;
            colors[i * 3 + 1] = nodeColor.y;
            colors[i * 3 + 2] = nodeColor.z;
        }
        _scene->meshes[meshId]->uploadColors(colors);
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
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                _mouseX = mouseX;
                _mouseY = mouseY;
                uint32_t pickedId = _scene->picking(_mouseX, _mouseY);
                if (pickedId != 0)
                {
                    _animMesh = _meshes[pickedId - 1];
                    _renderMesh = _scene->meshes[pickedId - 1];
                }
                _coloredMeshes();
            }
            else if (action == GLFW_RELEASE)
            {
                _leftButtonPressed = false;
                if (_animMesh)
                {
                    _animMesh->boundingBox->update();
                    _checkCollisions();
                    _coloredMeshes();
                    std::cout << "Number of collisions: " << _aabbs.size()
                              << std::endl;
                    _animMesh = nullptr;
                    _renderMesh = nullptr;
                }
            }
        }
        if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            if (action == GLFW_PRESS)
            {
                _middleButtonPressed = true;

                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                _mouseX = mouseX;
                _mouseY = mouseY;
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

                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                _mouseX = mouseX;
                _mouseY = mouseY;
            }
            else if (action == GLFW_RELEASE)
            {
                _rightButtonPressed = false;
            }
        }
    }
}

void DemoApp::_mousePositionCallback(GLFWwindow* window, float xpos, float ypos)
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
