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

#include <iomanip>
#include <iostream>
#include <thread>

#include "../common/Circuit.h"
#include "../common/CollisionSolver.h"
#include "../common/Morpho.h"

int main(int argc, char* argv[])
{
    std::string circuitPath;

    auto limits = new phyanim::geometry::AxisAlignedBoundingBox();
    limits->lowerLimit(phyanim::geometry::Vec3(-1000, -1000, -1000));
    limits->upperLimit(phyanim::geometry::Vec3(1000, 1000, 1000));

    bbp::sonata::Selection::Values ids;
    std::string pop = "All";

    float threshold = 1.0f;
    float dt = 0.0001f;

    for (uint32_t i = 1; i < argc; ++i)
    {
        size_t extensionPos;

        std::string arg(argv[i]);

        if (arg.compare("-l") == 0)
        {
            ++i;
            float x = std::atof(argv[i]);
            ++i;
            float y = std::atof(argv[i]);
            ++i;
            float z = std::atof(argv[i]);
            limits->lowerLimit(phyanim::geometry::Vec3(x, y, z));
        }
        else if (arg.compare("-u") == 0)
        {
            ++i;
            float x = std::atof(argv[i]);
            ++i;
            float y = std::atof(argv[i]);
            ++i;
            float z = std::atof(argv[i]);
            limits->upperLimit(phyanim::geometry::Vec3(x, y, z));
        }
        else if (arg.compare("-p") == 0)
        {
            ++i;
            pop = std::string(argv[i]);
        }
        else if (arg.compare("-t") == 0)
        {
            ++i;
            threshold = std::atof(argv[i]);
        }
        else if (arg.find(".json") != std::string::npos)
            circuitPath = arg;
        else
            ids.push_back(std::stoul(arg));

        // std::cerr << "Unknown file format: " << _args[i] << std::endl;
    }
    auto solver = new examples::CollisionSolver(dt);

    examples::Circuit circuit(circuitPath, pop);
    std::cout << "Number of morphologies to load: " << ids.size() << std::endl;
    std::vector<examples::Morpho*> morphologies =
        circuit.getNeurons(ids, limits);
    uint32_t size = morphologies.size();
    std::cout << "Number of morphologies loaded: " << size << std::endl;

    phyanim::geometry::HierarchicalAABBs morphoAABBs(size);
    std::vector<phyanim::geometry::Edges> edgesSet(size);
    std::vector<phyanim::geometry::Nodes> nodesSet(size);

    // uint32_t sizeEdges = 0;
    // uint32_t sizeNodes = 0;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < size; ++i)
    {
        edgesSet[i] = morphologies[i]->edges;
        phyanim::geometry::resample(edgesSet[i], 0.15);
        phyanim::geometry::removeOutEdges(edgesSet[i], *limits);
        nodesSet[i] = phyanim::geometry::uniqueNodes(edgesSet[i]);
        morphoAABBs[i] = new phyanim::geometry::HierarchicalAABB(edgesSet[i]);
        // #ifdef PHYANIM_USES_OPENMP
        // #pragma omp critical
        // #endif
        //         {
        //             sizeEdges += edgesSet[i].size();
        //             sizeNodes += nodesSet[i].size();
        //         }
    }

    // std::cout << "Simulation with " << sizeEdges << " springs and " <<
    // sizeNodes
    //           << " nodes" << std::endl;

    // auto startTime = std::chrono::steady_clock::now();

    uint32_t totalIters = 0;
    uint32_t cols = solver->solveCollisions(morphoAABBs, edgesSet, nodesSet,
                                            *limits, totalIters, threshold);

    // auto endTime = std::chrono::steady_clock::now();
    // std::chrono::duration<float> elapsedTime = endTime - startTime;

    // std::cout << cols << " collisions in " << totalIters << " iters and "
    //           << elapsedTime.count() << " seconds." << std::endl;
}
