
#include <iomanip>
#include <iostream>
#include <thread>

#include "../common/Circuit.h"
#include "../common/CollisionSolver.h"
#include "../common/Morpho.h"

int main(int argc, char* argv[])
{
    std::string circuitPath;

    auto limits = new phyanim::AxisAlignedBoundingBox();
    limits->lowerLimit(phyanim::Vec3(-1000, -1000, -1000));
    limits->upperLimit(phyanim::Vec3(1000, 1000, 1000));

    bbp::sonata::Selection::Values ids;
    std::string pop = "All";

    for (uint32_t i = 1; i < argc; ++i)
    {
        size_t extensionPos;

        std::string arg(argv[i]);

        if (arg.compare("-l") == 0)
        {
            ++i;
            double x = std::atof(argv[i]);
            ++i;
            double y = std::atof(argv[i]);
            ++i;
            double z = std::atof(argv[i]);
            limits->lowerLimit(phyanim::Vec3(x, y, z));
        }
        else if (arg.compare("-u") == 0)
        {
            ++i;
            double x = std::atof(argv[i]);
            ++i;
            double y = std::atof(argv[i]);
            ++i;
            double z = std::atof(argv[i]);
            limits->upperLimit(phyanim::Vec3(x, y, z));
        }
        else if (arg.compare("-p") == 0)
        {
            ++i;
            pop = std::string(argv[i]);
        }
        else if (arg.find(".json") != std::string::npos)
            circuitPath = arg;
        else
            ids.push_back(std::stoul(arg));

        // std::cerr << "Unknown file format: " << _args[i] << std::endl;
    }
    std::cout << "Number of morphologies to load: " << ids.size() << std::endl;

    auto solver = new examples::CollisionSolver(0.001);

    examples::Circuit circuit(circuitPath, pop);
    std::vector<examples::Morpho*> morphologies =
        circuit.getNeurons(ids, limits);
    uint32_t size = morphologies.size();
    std::cout << "Number of morphologies loaded: " << size << std::endl;

    phyanim::HierarchicalAABBs morphoAABBs(size);
    std::vector<phyanim::Edges> edgesSet(size);
    std::vector<phyanim::Nodes> nodesSet(size);

    // uint32_t sizeEdges = 0;
    // uint32_t sizeNodes = 0;
#ifdef PHYANIM_USES_OPENMP
#pragma omp parallel for
#endif
    for (uint32_t i = 0; i < size; ++i)
    {
        edgesSet[i] = morphologies[i]->edges;
        resample(edgesSet[i], 0.15);
        removeOutEdges(edgesSet[i], *limits);
        nodesSet[i] = uniqueNodes(edgesSet[i]);
        morphoAABBs[i] = new phyanim::HierarchicalAABB(edgesSet[i]);
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
                                            *limits, totalIters);

    // auto endTime = std::chrono::steady_clock::now();
    // std::chrono::duration<double> elapsedTime = endTime - startTime;

    // std::cout << cols << " collisions in " << totalIters << " iters and "
    //           << elapsedTime.count() << " seconds." << std::endl;
}
