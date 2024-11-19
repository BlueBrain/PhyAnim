/* Copyright (c) 2020-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Juan Jose Garcia <juanjose.garcia@epfl.ch>
 *
 * This file is part of PhyAnim <https://github.com/BlueBrain/PhyAnim>
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
 
#include "SomaApp.h"

#include <chrono>
#include <iostream>

#include "../common/Morpho.h"
#include "SomaGenerator.h"

int main(int argc, char* argv[])
{
    examples::SomaApp app(argc, argv);
    app.run();

    return 0;
}

namespace examples
{
SomaApp::SomaApp(int argc, char** argv) : GLFWApp(argc, argv), _anim(false) {}

void SomaApp::_actionLoop()
{
    float dt = 0.01;
    float stiffness = 1000.0f;
    float poissonRatio = 0.2;
    float alphaSoma = 0.75;
    uint32_t iters = 100;
    uint32_t iter = 0;
    bool offline = false;
    std::string file("data/a_s.swc");

    for (int i = 0; i < _args.size(); ++i)
    {
        std::string option(_args[i]);
        try
        {
            if (option.compare("-dt") == 0)
            {
                ++i;
                dt = std::stof(_args[i]);
            }
            else if (option.compare("-ks") == 0)
            {
                ++i;
                stiffness = std::stof(_args[i]);
            }
            else if (option.compare("-as") == 0)
            {
                ++i;
                alphaSoma = std::stof(_args[i]);
            }
            else if (option.compare("-pr") == 0)
            {
                ++i;
                poissonRatio = std::stof(_args[i]);
            }
            else if (option.compare("-it") == 0)
            {
                ++i;
                iters = std::stof(_args[i]);
            }
            else if (option.compare("-off") == 0)
            {
                offline = true;
            }
            else
            {
                file = option;
            }
        }
        catch (...)
        {
            std::cerr << "Usage error\n" << std::endl;
            exit(-1);
        }
    }

    auto morpho = examples::Morpho(file, phyanim::geometry::Mat4(1.0f),
                                   examples::RadiusFunc::MIN_NEURITES);

    if (morpho.sectionNodes.empty()) throw std::runtime_error("Empty neurites");
    phyanim::geometry::Vec3 center;
    float radius = std::numeric_limits<float>::max();
    for (auto node : morpho.sectionNodes) center += node->position;
    center /= morpho.sectionNodes.size();
    for (auto node : morpho.sectionNodes)
    {
        float dist = glm::distance(center, node->position);
        radius = std::min(radius, dist);
    }
    std::cout << "Soma center " << center.x << ", " << center.y << ", "
              << center.z << std::endl;
    std::cout << "Soma radius " << radius << std::endl;
    for (auto neurite : morpho.sectionNodes)
    {
        auto p = neurite->position;
        std::cout << "---Neurite pos: " << p.x << ", " << p.y << ", " << p.z
                  << " radius: " << neurite->radius * 0.5 << std::endl;
    }

    SomaGenerator generator(center, radius * alphaSoma, morpho.sectionNodes, dt,
                            stiffness, poissonRatio);

    _meshes.push_back(generator.animMesh);
    _scene->meshes.push_back(generator.renderMesh);

    geometry::AxisAlignedBoundingBox limits(
        generator.animMesh->surfaceTriangles);
    limits.resize(2.0f);
    _setCameraPos(limits);

    std::cout << "dt: " << dt << std::endl;
    std::cout << "stiffness: " << stiffness << std::endl;
    std::cout << "poissonRatio: " << poissonRatio << std::endl;
    std::cout << "alphaSoma: " << alphaSoma << std::endl;

    std::cout << "Progress: " << iter / iters << "%" << std::flush;

    _anim = true;
    bool anim = _anim;
    while (true)
    {
        if (anim)
        {
            ++iter;
            if (iter <= iters)
            {
                float alpha = (float)iter / iters;
                generator.pull(alpha);
                generator.anim(true);
                std::cout << "\33[2K\rProgress: " << iter * 100.0f / iters
                          << "%" << std::flush;
                if (iter == iters) std::cout << std::endl;
            }
            else
            {
                generator.anim(true);
            }
        }
        _animMutex.lock();
        anim = _anim;
        _animMutex.unlock();
    }
}

void SomaApp::_keyCallback(GLFWwindow* window,
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
            case GLFW_KEY_SPACE:
                _animMutex.lock();
                _anim = !_anim;
                _animMutex.unlock();
                if (_anim)
                    std::cout << "\nsimulation released" << std::endl;
                else
                    std::cout << "\nsimulation paused" << std::endl;
                break;
            }
        }
    }
}

}  // namespace examples
