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

#include <iomanip>
#include <iostream>
#include <thread>

#include "../common/Circuit.h"
#include "../common/CollisionSolver.h"

using namespace phyanim;

int main(int argc, char* argv[])
{
    examples::RadiusFunc radiusFunc = examples::RadiusFunc::MIN_NEURITES;

    std::string circuitPath;
    bbp::sonata::Selection::Values ids;
    std::string pop = "All";

    float dt = 0.001f;
    float ksc = 10.0f;

    for (uint32_t i = 1; i < argc; ++i)
    {
        size_t extensionPos;

        std::string arg(argv[i]);

        if (arg.compare("-p") == 0)
        {
            ++i;
            pop = std::string(argv[i]);
        }
        else if (arg.compare("-minNeurites") == 0)
            radiusFunc = examples::RadiusFunc::MIN_NEURITES;
        else if (arg.compare("-maxNeurites") == 0)
            radiusFunc = examples::RadiusFunc::MAX_NEURITES;
        else if (arg.compare("-meanNeurites") == 0)
            radiusFunc = examples::RadiusFunc::MEAN_NEURITES;
        else if (arg.compare("-minSomas") == 0)
            radiusFunc = examples::RadiusFunc::MIN_SOMAS;
        else if (arg.compare("-maxSomas") == 0)
            radiusFunc = examples::RadiusFunc::MAX_SOMAS;
        else if (arg.compare("-meanSomas") == 0)
            radiusFunc = examples::RadiusFunc::MEAN_SOMAS;
        else if (arg.find(".json") != std::string::npos)
            circuitPath = arg;
        else if (arg.compare("-dt") == 0)
        {
            ++i;
            dt = std::stof(argv[i]);
        }
        else if (arg.compare("-ksc") == 0)
        {
            ++i;
            ksc = std::stof(argv[i]);
        }
        else
            ids.push_back(std::stoul(arg));
    }
    auto solver = new examples::CollisionSolver(dt);
    examples::Circuit circuit(circuitPath, pop);
    std::vector<examples::Morpho*> morphologies =
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
    }

    uint32_t size = morphologies.size();
    std::vector<float> factor(size);
    std::vector<geometry::Edges> edgesSet(size);
    std::vector<geometry::Nodes> nodesSet(size);
    geometry::HierarchicalAABBs aabbs(size);

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

    solver->solveSomasCollisions(aabbs, edgesSet, nodesSet, ksc);
}
