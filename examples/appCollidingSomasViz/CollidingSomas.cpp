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

#include "CollidingSomas.h"

#include <iomanip>
#include <iostream>
#include <thread>

#include "../common/Circuit.h"

using namespace phyanim;

int main(int argc, char* argv[])
{
    examples::CollidingSomas app(argc, argv);
    app.run();

    return 0;
}

namespace examples
{
CollidingSomas::CollidingSomas(int argc, char** argv)
    : GLFWApp(argc, argv)
    , _dt(0.001f)
    , _ksc(10.0f)
{
    _palette = new graphics::ColorPalette(graphics::CONTRAST);
}

void CollidingSomas::_actionLoop()
{
    RadiusFunc radiusFunc = RadiusFunc::MIN_NEURITES;

    _limits.lowerLimit(geometry::Vec3(-1000, -1000, -1000));
    _limits.upperLimit(geometry::Vec3(1000, 1000, 1000));

    std::string circuitPath;
    bbp::sonata::Selection::Values ids;
    std::string pop = "All";

    for (uint32_t i = 0; i < _args.size(); ++i)
    {
        size_t extensionPos;
        if (_args[i].compare("-l") == 0)
        {
            ++i;
            float x = std::stod(_args[i]);
            ++i;
            float y = std::stod(_args[i]);
            ++i;
            float z = std::stod(_args[i]);
            _limits.lowerLimit(geometry::Vec3(x, y, z));
        }
        else if (_args[i].compare("-u") == 0)
        {
            ++i;
            float x = std::stod(_args[i]);
            ++i;
            float y = std::stod(_args[i]);
            ++i;
            float z = std::stod(_args[i]);
            _limits.upperLimit(geometry::Vec3(x, y, z));
        }
        else if (_args[i].compare("-p") == 0)
        {
            ++i;
            pop = _args[i];
        }
        else if (_args[i].compare("-minNeurites") == 0)
            radiusFunc = RadiusFunc::MIN_NEURITES;
        else if (_args[i].compare("-maxNeurites") == 0)
            radiusFunc = RadiusFunc::MAX_NEURITES;
        else if (_args[i].compare("-meanNeurites") == 0)
            radiusFunc = RadiusFunc::MEAN_NEURITES;
        else if (_args[i].compare("-minSomas") == 0)
            radiusFunc = RadiusFunc::MIN_SOMAS;
        else if (_args[i].compare("-maxSomas") == 0)
            radiusFunc = RadiusFunc::MAX_SOMAS;
        else if (_args[i].compare("-meanSomas") == 0)
            radiusFunc = RadiusFunc::MEAN_SOMAS;
        else if (_args[i].compare("-dt") == 0)
        {
            ++i;
            _dt = std::stof(_args[i]);
        }
        else if (_args[i].compare("-ksc") == 0)
        {
            ++i;
            _ksc = std::stof(_args[i]);
        }
        else if (_args[i].find(".json") != std::string::npos)
            circuitPath = _args[i];
        else
            ids.push_back(std::stoul(_args[i]));
    }
    _solver = new CollisionSolver(_dt);

    _setCameraPos(_limits);
    Circuit circuit(circuitPath, pop);
    std::vector<Morpho*> morphologies =
        circuit.getNeurons(ids, nullptr, radiusFunc, false);

    std::cout << morphologies.size() << " morphologies loaded" << std::endl;

    for (uint32_t i = 0; i < morphologies.size(); ++i)
    {
        auto morpho = morphologies[i];
        if (!morpho->soma)
        {
            delete morpho;
            morphologies.erase(morphologies.begin() + i);
            --i;
        }
        else
        {
            morpho->soma->isSoma = false;
            morpho->soma->collide = false;
        }
    }

    uint32_t size = morphologies.size();
    std::vector<float> factor(size);
    std::vector<geometry::Edges> edgesSet(size);
    std::vector<geometry::Nodes> nodesSet(size);
    geometry::HierarchicalAABBs aabbs(size);
    // Check soma collisions

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < size; ++i)
    {
        auto soma0 = morphologies[i]->soma;
        for (auto node : morphologies[i]->sectionNodes)
        {
            auto nodeCenter = new geometry::Node(
                soma0->position, 0, node->radius, geometry::Vec3(),
                geometry::Vec3(), node->radius);
            edgesSet[i].push_back(new geometry::Edge(nodeCenter, node));
        }
        edgesSet[i].push_back(new geometry::Edge(soma0, soma0));
        nodesSet[i] = geometry::uniqueNodes(edgesSet[i]);
        aabbs[i] = new geometry::HierarchicalAABB(edgesSet[i]);
    }

    _setMeshes(edgesSet);

    _solveCollisions(aabbs, edgesSet, nodesSet);
}

uint32_t CollidingSomas::_solveCollisions(
    geometry::HierarchicalAABBs& aabbs,
    std::vector<geometry::Edges>& edgesSet,
    std::vector<geometry::Nodes>& nodesSet)
{
    uint32_t numCollisions = 1;
    uint32_t iter = 0;

    auto startTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime;

    while (numCollisions > 0)
    {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (uint32_t i = 0; i < nodesSet.size(); ++i)
        {
            geometry::clearCollision(nodesSet[i]);
            geometry::clearForce(nodesSet[i]);
        }
        numCollisions =
            anim::CollisionDetection::computeCollisions(aabbs, _ksc);

        _solver->animSomas(aabbs, nodesSet);

        if (iter % 100 == 0 | numCollisions == 0)
        {
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
            for (uint32_t i = 0; i < aabbs.size(); ++i)
            {
                updateGeometry(_scene->meshes[i], edgesSet[i],
                               _palette->color(i) * 0.3f, _palette->color(i),
                               _palette->color(i) * 0.2f);
            }
            elapsedTime = std::chrono::steady_clock::now() - startTime;
            std::cout << "Iter: " << iter << "  Collisions: " << numCollisions
                      << "  Time: " << elapsedTime.count() << " seconds."
                      << std::endl;
        }

        ++iter;
    }
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < aabbs.size(); ++i)
    {
        updateGeometry(_scene->meshes[i], edgesSet[i],
                       _palette->color(i) * 0.3f, _palette->color(i),
                       _palette->color(i) * 0.2f);
    }

    return iter;
}

void CollidingSomas::_setMeshes(std::vector<geometry::Edges>& edgesSet)
{
    for (auto mesh : _scene->meshes)
        if (mesh) delete mesh;
    _scene->meshes.clear();
    _scene->meshes.resize(edgesSet.size());

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < edgesSet.size(); ++i)
    {
        _scene->meshes[i] = graphics::generateMesh(
            edgesSet[i], _palette->color(i) * 0.3f, _palette->color(i),
            _palette->color(i) * 0.2f);
    }
}

void CollidingSomas::_mouseButtonCallback(GLFWwindow* window,
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
