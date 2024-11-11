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

#ifndef __EXAMPLES_CIRCUIT_H
#define __EXAMPLES_CIRCUIT_H

#include <bbp/sonata/config.h>

#include <iomanip>
#include <iostream>

#include "Morpho.h"

namespace examples
{

class Circuit
{
public:
    Circuit(std::string path, std::string populationName)
        : _circuit(path)
        , _population(populationName)
    {
        std::string homePath(realpath(_circuit.c_str(), nullptr));
        homePath = homePath.substr(0, homePath.find_last_of('/') + 1);
        auto config = bbp::sonata::CircuitConfig::fromFile(_circuit);
        auto properties = config.getNodePopulationProperties(_population);
        _morphoDir = properties.morphologiesDir;
        _morphoExt = ".swc";
        if (_morphoDir.compare(homePath) == 0)
        {
            _morphoDir = "";
            _morphoExt = "";
            auto alternateDir = properties.alternateMorphologyFormats;
            if (!alternateDir.empty())
            {
                _morphoDir = alternateDir.begin()->second + "/";
                if (alternateDir.begin()->first.compare("neurolucida-asc") == 0)
                    _morphoExt = ".asc";
                else if (alternateDir.begin()->first.compare("h5v1") == 0)
                    _morphoExt = ".h5";
            }
        }
        // std::cout << "morphology folder: " << _morphoDir
        //           << "\nmorphology extension: " << _morphoExt << std::endl;
    };

    ~Circuit(){};

    Morpho* getNeuron(uint32_t id,
                      geometry::AxisAlignedBoundingBox* aabb = nullptr)
    {
        bbp::sonata::Selection::Values ids;
        ids.push_back(id);
        auto selection = bbp::sonata::Selection::fromValues(ids);
        auto config = bbp::sonata::CircuitConfig::fromFile(_circuit);
        auto population = config.getNodePopulation(_population);
        auto morphoPaths =
            population.getAttribute<std::string>("morphology", selection);

        std::string path = _morphoDir + morphoPaths[0] + _morphoExt;
        auto morpho = new Morpho(path);
        if (aabb) morpho->cutout(*aabb);
        return morpho;
    };

    std::vector<Morpho*> getNeurons(
        bbp::sonata::Selection::Values ids,
        geometry::AxisAlignedBoundingBox* aabb = nullptr,
        RadiusFunc radiusFunc = RadiusFunc::MAX_NEURITES,
        bool loadNeurites = true)
    {
        auto selection = bbp::sonata::Selection::fromValues(ids);
        auto config = bbp::sonata::CircuitConfig::fromFile(_circuit);
        auto population = config.getNodePopulation(_population);
        auto morphoPaths =
            population.getAttribute<std::string>("morphology", selection);
        auto xs = population.getAttribute<float>("x", selection);
        auto ys = population.getAttribute<float>("y", selection);
        auto zs = population.getAttribute<float>("z", selection);

        auto rot_xs =
            population.getAttribute<float>("orientation_x", selection);
        auto rot_ys =
            population.getAttribute<float>("orientation_y", selection);
        auto rot_zs =
            population.getAttribute<float>("orientation_z", selection);
        auto rot_ws =
            population.getAttribute<float>("orientation_w", selection);

        uint32_t num = morphoPaths.size();
        std::vector<Morpho*> morphos(num);
        uint32_t loaded = 0;
        // std::cout << std::setprecision(4) << std::fixed;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (uint32_t i = 0; i < num; ++i)
        {
            std::string path = _morphoDir + morphoPaths[i] + _morphoExt;
            geometry::Vec3 pos(xs[i], ys[i], zs[i]);
            glm::quat rot(rot_ws[i], rot_xs[i], rot_ys[i], rot_zs[i]);
            geometry::Mat4 model(1);
            model = glm::translate(model, pos) *
                    geometry::Mat4(glm::normalize(rot));

            morphos[i] = new Morpho(path, model, radiusFunc, loadNeurites);
            if (aabb) morphos[i]->cutout(*aabb);
#pragma omp critical
            {
                loaded++;
                std::cout << "\r\e[K" << std::flush;
                std::cout << "\rLoading " << (float)(loaded * 100.0 / num)
                          << "%" << std::flush;
            }
        }
        std::cout << std::endl;
        return morphos;
    };

    void collidingSomas(bbp::sonata::Selection::Values ids,
                        geometry::AxisAlignedBoundingBox* aabb)
    {
        auto selection = bbp::sonata::Selection::fromValues(ids);
        auto config = bbp::sonata::CircuitConfig::fromFile(_circuit);
        auto population = config.getNodePopulation(_population);
        auto morphoPaths =
            population.getAttribute<std::string>("morphology", selection);
        auto xs = population.getAttribute<float>("x", selection);
        auto ys = population.getAttribute<float>("y", selection);
        auto zs = population.getAttribute<float>("z", selection);

        auto rot_xs =
            population.getAttribute<float>("orientation_x", selection);
        auto rot_ys =
            population.getAttribute<float>("orientation_y", selection);
        auto rot_zs =
            population.getAttribute<float>("orientation_z", selection);
        auto rot_ws =
            population.getAttribute<float>("orientation_w", selection);

        uint32_t num = morphoPaths.size();
        std::vector<Morpho*> morphos(num);
        uint32_t loaded = 0;
        // std::cout << std::setprecision(4) << std::fixed;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (uint32_t i = 0; i < num; ++i)
        {
            std::string path = _morphoDir + morphoPaths[i] + _morphoExt;
            geometry::Vec3 pos(xs[i], ys[i], zs[i]);
            glm::quat rot(rot_ws[i], rot_xs[i], rot_ys[i], rot_zs[i]);
            geometry::Mat4 model(1);
            model = glm::translate(model, pos) *
                    geometry::Mat4(glm::normalize(rot));

            morphos[i] = new Morpho(path, model);
            if (aabb) morphos[i]->cutout(*aabb);
            bool isColliding = aabb->isColliding(*morphos[i]->soma);
#pragma omp critical
            {
                ++loaded;
                if (isColliding)
                    std::cout << "\rId: " << ids[i]
                              << " radius: " << morphos[i]->soma->radius
                              << std::endl;
                std::cout << "\r\e[K" << std::flush;
                std::cout << "\rLoading " << (float)(loaded * 100.0 / num)
                          << "%" << std::flush;
            }
        }
        std::cout << std::endl;
    };

private:
    std::string _circuit;
    std::string _population;
    std::string _morphoDir;
    std::string _morphoExt;
};

}  // namespace examples

#endif
