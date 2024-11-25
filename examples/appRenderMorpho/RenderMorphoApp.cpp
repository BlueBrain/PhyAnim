/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "RenderMorphoApp.h"

#include <iomanip>
#include <iostream>
#include <thread>

int main(int argc, char* argv[])
{
    examples::RenderMorphoApp app(argc, argv);
    app.run();

    return 0;
}

namespace examples
{
RenderMorphoApp::RenderMorphoApp(int argc, char** argv) : GLFWApp(argc, argv) {}

void RenderMorphoApp::_actionLoop()
{
    std::vector<std::string> files;
    for (uint32_t i = 0; i < _args.size(); ++i)
    {
        size_t extensionPos;
        if (_args[i].find(".swc") != std::string::npos)
        {
            files.push_back(_args[i]);
        }
        else
            std::cerr << "Unknown file format: " << _args[i] << std::endl;
    }

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < files.size(); ++i)
    {
        auto morpho = new Morpho(files[i]);
        auto mesh = graphics::generateMesh(morpho->edges);

#pragma omp critical
        {
            _morphologies.push_back(morpho);
            _scene->meshes.push_back(mesh);
            _limits.unite(*morpho->aabb);
            _setCameraPos(_limits);
        }
    }

    // auto prevTime = std::chrono::steady_clock::now();
    // uint32_t iters = 0;

    // uint32_t numSprings = 0;
    // for (uint32_t i = 0; i < _morphologies.size(); ++i)
    //     numSprings += _morphologies[i]->edges.size();
}

void RenderMorphoApp::_mouseButtonCallback(GLFWwindow* window,
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
            }
            else if (action == GLFW_RELEASE)
            {
                _leftButtonPressed = false;
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

}  // namespace examples
