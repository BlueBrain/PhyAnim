#ifndef __EXAMPLES_CIRCUIT_H
#define __EXAMPLES_CIRCUIT_H

#include <bbp/sonata/config.h>

// #include <spatial_index/multi_index.hpp>
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
                      phyanim::AxisAlignedBoundingBox* aabb = nullptr)
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
        phyanim::AxisAlignedBoundingBox* aabb = nullptr)
    {
        auto selection = bbp::sonata::Selection::fromValues(ids);
        auto config = bbp::sonata::CircuitConfig::fromFile(_circuit);
        auto population = config.getNodePopulation(_population);
        auto morphoPaths =
            population.getAttribute<std::string>("morphology", selection);
        auto xs = population.getAttribute<double>("x", selection);
        auto ys = population.getAttribute<double>("y", selection);
        auto zs = population.getAttribute<double>("z", selection);

        auto rot_xs =
            population.getAttribute<double>("orientation_x", selection);
        auto rot_ys =
            population.getAttribute<double>("orientation_y", selection);
        auto rot_zs =
            population.getAttribute<double>("orientation_z", selection);
        auto rot_ws =
            population.getAttribute<double>("orientation_w", selection);

        uint32_t num = morphoPaths.size();
        std::vector<Morpho*> morphos(num);
        uint32_t loaded = 0;
        std::cout << std::setprecision(2) << std::fixed;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
        for (uint32_t i = 0; i < num; ++i)
        {
            std::string path = _morphoDir + morphoPaths[i] + _morphoExt;
            phyanim::Vec3 pos(xs[i], ys[i], zs[i]);
            Eigen::Quaterniond rot(rot_ws[i], rot_xs[i], rot_ys[i], rot_zs[i]);
            rot.normalize();
            phyanim::Mat4 model = phyanim::Mat4::Identity();
            model.block<3, 3>(0, 0) = rot.toRotationMatrix();
            model.block<3, 1>(0, 3) = pos;
            morphos[i] = new Morpho(path, model);
            if (aabb) morphos[i]->cutout(*aabb);
#pragma omp critical
            {
                loaded++;
                std::cout << "\r\e[K" << std::flush;
                std::cout << "\rLoading " << (double)(loaded * 100.0 / num)
                          << "%" << std::flush;
            }
        }
        std::cout << std::endl;
        return morphos;
    };

private:
    std::string _circuit;
    std::string _population;
    std::string _morphoDir;
    std::string _morphoExt;
};

}  // namespace examples

#endif
