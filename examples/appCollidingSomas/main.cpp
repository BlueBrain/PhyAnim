
#include <iomanip>
#include <iostream>
#include <thread>

#include "../common/Circuit.h"
#include "../common/CollisionSolver.h"
#include "../common/Morpho.h"

int main(int argc, char* argv[])
{
    examples::RadiusFunc radiusFunc = examples::RadiusFunc::MIN_NEURITES;

    std::string circuitPath;
    bbp::sonata::Selection::Values ids;
    std::string pop = "All";

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
        else
            ids.push_back(std::stoul(arg));
    }
    auto solver = new examples::CollisionSolver(0.001);
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
    std::vector<double> factor(size);
    std::vector<phyanim::Edges> edgesSet(size);
    std::vector<phyanim::Nodes> nodesSet(size);
    phyanim::HierarchicalAABBs aabbs(size);

#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < size; ++i)
    {
        auto soma0 = morphologies[i]->soma;
        for (auto node : morphologies[i]->sectionNodes)
        {
            auto nodeCenter = new phyanim::Node(
                soma0->position, 0, node->radius, phyanim::Vec3::Zero(),
                phyanim::Vec3::Zero(), node->radius);
            edgesSet[i].push_back(new phyanim::Edge(nodeCenter, node));
        }
        edgesSet[i].push_back(new phyanim::Edge(soma0, soma0));
        nodesSet[i] = phyanim::uniqueNodes(edgesSet[i]);
        aabbs[i] = new phyanim::HierarchicalAABB(edgesSet[i]);
    }

    solver->solveSomasCollisions(aabbs, edgesSet, nodesSet);
}
